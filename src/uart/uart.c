#include "uart/uart.h"
#include "bsp/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include <string.h>

/* Timeout sized well above worst case: 128 bytes @ 2 Mbaud = ~0.5 ms. */
#define UART_DMA_TIMEOUT_MS  50U

typedef struct {
    uint8_t  data[UART_TX_MAX_MSG_LEN];
    uint16_t len;
} uart_msg_t;

static QueueHandle_t     tx_queue;
static SemaphoreHandle_t tx_complete;
static uint32_t          drop_count;

static void tx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(tx_complete, &woken);
    portYIELD_FROM_ISR(woken);
}

static void uart_drain_task(void *arg)
{
    (void)arg;
    uart_msg_t msg;

    for (;;) {
        xQueueReceive(tx_queue, &msg, portMAX_DELAY);
        if (bsp_uart_transmit_dma(msg.data, msg.len)) {
            xSemaphoreTake(tx_complete, pdMS_TO_TICKS(UART_DMA_TIMEOUT_MS));
        } else {
            drop_count++;
        }
    }
}

void uart_init(void)
{
    tx_complete = xSemaphoreCreateBinary();
    tx_queue    = xQueueCreate(UART_TX_QUEUE_DEPTH, sizeof(uart_msg_t));
    configASSERT(tx_complete);
    configASSERT(tx_queue);
    bsp_uart_set_tx_cplt_cb(tx_cplt_handler);
    xTaskCreate(uart_drain_task, "UART_drain", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}

uint32_t uart_get_drop_count(void)
{
    uint32_t n = drop_count;
    drop_count = 0;
    return n;
}

void uart_transmit(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return;

    uart_msg_t msg;
    msg.len = len > UART_TX_MAX_MSG_LEN ? UART_TX_MAX_MSG_LEN : len;
    memcpy(msg.data, buf, msg.len);

    /* Blocks only if all UART_TX_QUEUE_DEPTH slots are full. */
    xQueueSend(tx_queue, &msg, portMAX_DELAY);
}
