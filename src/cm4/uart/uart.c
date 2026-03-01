#include "uart.h"
#include "bsp/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* Timeout sized well above worst case: 256 bytes @ 2 Mbaud = ~1 ms. */
#define UART_DMA_TIMEOUT_MS  50U

static SemaphoreHandle_t tx_complete;
static SemaphoreHandle_t tx_mutex;

static void tx_cplt_handler(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(tx_complete, &woken);
    portYIELD_FROM_ISR(woken);
}

void uart_init(void)
{
    tx_complete = xSemaphoreCreateBinary();
    tx_mutex    = xSemaphoreCreateMutex();
    configASSERT(tx_complete);
    configASSERT(tx_mutex);
    bsp_uart_set_tx_cplt_cb(tx_cplt_handler);
}

void uart_transmit(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return;

    /* Serialise concurrent callers — only one DMA transfer at a time. */
    xSemaphoreTake(tx_mutex, portMAX_DELAY);

    if (bsp_uart_transmit_dma(buf, len))
    {
        /* Block until TC fires or timeout (guards against hardware faults). */
        xSemaphoreTake(tx_complete, pdMS_TO_TICKS(UART_DMA_TIMEOUT_MS));
    }

    xSemaphoreGive(tx_mutex);
}
