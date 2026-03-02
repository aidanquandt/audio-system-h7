#include "uart/uart.h"
#include "bsp/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include <string.h>

#define UART_DMA_TIMEOUT_MS  50U

typedef struct {
    uint8_t  data[UART_TX_MAX_MSG_LEN];
    uint16_t len;
} uart_msg_t;

static QueueHandle_t     tx_queue;
static SemaphoreHandle_t tx_complete;
static uint32_t          drop_count;

static SemaphoreHandle_t rx_ready;
static uint8_t           rx_buf[UART_RX_BUF_LEN];

static void tx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(tx_complete, &woken);
    portYIELD_FROM_ISR(woken);
}

static void rx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(rx_ready, &woken);
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

static void uart_rx_task(void *arg)
{
    (void)arg;

    bsp_uart_receive_dma(rx_buf, UART_RX_BUF_LEN);

    for (;;) {
        xSemaphoreTake(rx_ready, portMAX_DELAY);
        uart_on_receive(rx_buf, UART_RX_BUF_LEN);
        bsp_uart_receive_dma(rx_buf, UART_RX_BUF_LEN);
    }
}

__attribute__((weak)) void uart_on_receive(const uint8_t *buf, uint16_t len)
{
    (void)buf;
    (void)len;
}

void uart_init(void)
{
    tx_complete = xSemaphoreCreateBinary();
    tx_queue    = xQueueCreate(UART_TX_QUEUE_DEPTH, sizeof(uart_msg_t));
    rx_ready    = xSemaphoreCreateBinary();
    configASSERT(tx_complete);
    configASSERT(tx_queue);
    configASSERT(rx_ready);
    bsp_uart_set_tx_cplt_cb(tx_cplt_handler);
    bsp_uart_set_rx_cplt_cb(rx_cplt_handler);
    xTaskCreate(uart_drain_task, "UART_tx", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(uart_rx_task,    "UART_rx", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
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

    xQueueSend(tx_queue, &msg, portMAX_DELAY);
}
