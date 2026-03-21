#include "drivers/uart/uart_driver.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"
#include <string.h>

#ifdef CORE_CM4

#include "usart.h"
#include <stdbool.h>

#define UART_DMA_TIMEOUT_MS 50U

typedef struct
{
    uint8_t data[UART_TX_MAX_MSG_LEN];
    uint16_t len;
} uart_msg_t;

static struct
{
    QueueHandle_t queue;
    SemaphoreHandle_t complete;
    uint32_t drop_count;
} tx_ctx;

static struct
{
    StreamBufferHandle_t stream;
    uint8_t buf[UART_RX_BUF_LEN];
    volatile uint32_t drop_count;
} rx_ctx;

/* HAL-level callbacks (set during init). */
static void (*uart_tx_cplt_cb)(void);
static void (*uart_rx_cplt_cb)(uint16_t);

static bool uart_hal_transmit_dma(const uint8_t* buf, uint16_t len)
{
    if (buf == NULL || len == 0)
    {
        return false;
    }
    return HAL_UART_Transmit_DMA(&huart3, (uint8_t*) buf, len) == HAL_OK;
}

static bool uart_hal_receive_dma(uint8_t* buf, uint16_t len)
{
    if (buf == NULL || len == 0)
    {
        return false;
    }
    return HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, len) == HAL_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != USART3)
    {
        return;
    }
    if (uart_tx_cplt_cb != NULL)
    {
        uart_tx_cplt_cb();
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    if (huart->Instance != USART3)
    {
        return;
    }
    if (uart_rx_cplt_cb != NULL)
    {
        uart_rx_cplt_cb(size);
    }
}

static void uart_driver_tx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(tx_ctx.complete, &woken);
    portYIELD_FROM_ISR(woken);
}

static void uart_driver_rx_cplt_handler(uint16_t received)
{
    BaseType_t woken = pdFALSE;
    size_t written   = xStreamBufferSendFromISR(rx_ctx.stream, rx_ctx.buf, received, &woken);
    if (written < received)
    {
        rx_ctx.drop_count += received - written;
    }
    uart_hal_receive_dma(rx_ctx.buf, UART_RX_BUF_LEN);
    portYIELD_FROM_ISR(woken);
}

static void uart_driver_drain_task(void* arg)
{
    (void) arg;
    uart_msg_t msg;

    for (;;)
    {
        xQueueReceive(tx_ctx.queue, &msg, portMAX_DELAY);
        if (uart_hal_transmit_dma(msg.data, msg.len))
        {
            if (!xSemaphoreTake(tx_ctx.complete, pdMS_TO_TICKS(UART_DMA_TIMEOUT_MS)))
            {
                tx_ctx.drop_count++;
            }
        }
        else
        {
            tx_ctx.drop_count++;
        }
    }
}

StreamBufferHandle_t uart_driver_get_rx_stream(void)
{
    return rx_ctx.stream;
}

void uart_driver_init(void)
{
    tx_ctx.complete = xSemaphoreCreateBinary();
    tx_ctx.queue    = xQueueCreate(UART_TX_QUEUE_DEPTH, sizeof(uart_msg_t));
    rx_ctx.stream   = xStreamBufferCreate(UART_RX_STREAM_SIZE, 1);
    configASSERT(tx_ctx.complete);
    configASSERT(tx_ctx.queue);
    configASSERT(rx_ctx.stream);
    uart_tx_cplt_cb = uart_driver_tx_cplt_handler;
    uart_rx_cplt_cb = uart_driver_rx_cplt_handler;
    bool ok         = uart_hal_receive_dma(rx_ctx.buf, UART_RX_BUF_LEN);
    configASSERT(ok);
    xTaskCreate(uart_driver_drain_task, "UART_tx", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}

uint32_t uart_driver_get_drop_count(void)
{
    uint32_t n_rx, n_tx;
    taskENTER_CRITICAL();
    n_rx              = rx_ctx.drop_count;
    rx_ctx.drop_count = 0;
    n_tx              = tx_ctx.drop_count;
    tx_ctx.drop_count = 0;
    taskEXIT_CRITICAL();
    return n_rx + n_tx;
}

void uart_driver_transmit(const uint8_t* buf, size_t len)
{
    if (buf == NULL || len == 0)
    {
        return;
    }

    if (len > UART_TX_MAX_MSG_LEN)
    {
        /* Truncating would emit a corrupted frame. Count as a drop and bail. */
        taskENTER_CRITICAL();
        tx_ctx.drop_count++;
        taskEXIT_CRITICAL();
        return;
    }

    uart_msg_t msg;
    msg.len = (uint16_t) len;
    memcpy(msg.data, buf, msg.len);

    xQueueSend(tx_ctx.queue, &msg, portMAX_DELAY);
}

#else

void uart_driver_init(void) {}
void uart_driver_transmit(const uint8_t* buf, size_t len)
{
    (void) buf;
    (void) len;
}
uint32_t uart_driver_get_drop_count(void)
{
    return 0U;
}
StreamBufferHandle_t uart_driver_get_rx_stream(void)
{
    return NULL;
}

#endif /* CORE_CM4 */
