#include "uart/uart.h"
#include "bsp/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "stream_buffer.h"
#include "task.h"
#include <string.h>

#define UART_DMA_TIMEOUT_MS  50U

typedef struct {
    uint8_t  data[UART_TX_MAX_MSG_LEN];
    uint16_t len;
} uart_msg_t;

static struct {
    QueueHandle_t     queue;
    SemaphoreHandle_t complete;
    uint32_t          drop_count;
} tx_ctx;

static struct {
    StreamBufferHandle_t stream;
    uint8_t              buf[UART_RX_BUF_LEN];
    volatile uint32_t    drop_count;
} rx_ctx;

static void tx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(tx_ctx.complete, &woken);
    portYIELD_FROM_ISR(woken);
}

static void rx_cplt_handler(uint16_t received)
{
    BaseType_t woken = pdFALSE;
    size_t written = xStreamBufferSendFromISR(rx_ctx.stream, rx_ctx.buf, received, &woken);
    if (written < received) rx_ctx.drop_count += received - written;
    bsp_uart_receive_dma(rx_ctx.buf, UART_RX_BUF_LEN);
    portYIELD_FROM_ISR(woken);
}

static void uart_drain_task(void *arg)
{
    (void)arg;
    uart_msg_t msg;

    for (;;) {
        xQueueReceive(tx_ctx.queue, &msg, portMAX_DELAY);
        if (bsp_uart_transmit_dma(msg.data, msg.len)) {
            if (!xSemaphoreTake(tx_ctx.complete, pdMS_TO_TICKS(UART_DMA_TIMEOUT_MS))) {
                tx_ctx.drop_count++;
            }
        } else {
            tx_ctx.drop_count++;
        }
    }
}

StreamBufferHandle_t uart_get_rx_stream(void)
{
    return rx_ctx.stream;
}

void uart_init(void)
{
    tx_ctx.complete = xSemaphoreCreateBinary();
    tx_ctx.queue    = xQueueCreate(UART_TX_QUEUE_DEPTH, sizeof(uart_msg_t));
    rx_ctx.stream   = xStreamBufferCreate(UART_RX_STREAM_SIZE, 1);
    configASSERT(tx_ctx.complete);
    configASSERT(tx_ctx.queue);
    configASSERT(rx_ctx.stream);
    bsp_uart_set_tx_cplt_cb(tx_cplt_handler);
    bsp_uart_set_rx_cplt_cb(rx_cplt_handler);
    bool ok = bsp_uart_receive_dma(rx_ctx.buf, UART_RX_BUF_LEN);
    configASSERT(ok);
    xTaskCreate(uart_drain_task, "UART_tx", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}

uint32_t uart_get_drop_count(void)
{
    uint32_t n_rx, n_tx;
    taskENTER_CRITICAL();
    n_rx = rx_ctx.drop_count; rx_ctx.drop_count = 0;
    taskEXIT_CRITICAL();
    n_tx = tx_ctx.drop_count; tx_ctx.drop_count = 0;
    return n_rx + n_tx;
}

void uart_transmit(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return;

    uart_msg_t msg;
    msg.len = len > UART_TX_MAX_MSG_LEN ? UART_TX_MAX_MSG_LEN : len;
    memcpy(msg.data, buf, msg.len);

    xQueueSend(tx_ctx.queue, &msg, portMAX_DELAY);
}
