#include "bsp/uart.h"

#ifdef CORE_CM4

#include "usart.h"
#include "FreeRTOS.h"
#include "semphr.h"

static SemaphoreHandle_t s_tx_done;
static SemaphoreHandle_t s_tx_mutex;

/* Timeout sized well above worst case: 256 bytes @ 2 Mbaud = ~1 ms. */
#define UART_DMA_TIMEOUT_MS  50U

void bsp_uart_init(void)
{
    s_tx_done  = xSemaphoreCreateBinary();
    s_tx_mutex = xSemaphoreCreateMutex();
    configASSERT(s_tx_done);
    configASSERT(s_tx_mutex);
}

void bsp_uart_transmit(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return;

    /* Serialise concurrent callers — only one DMA transfer at a time. */
    xSemaphoreTake(s_tx_mutex, portMAX_DELAY);

    if (HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buf, len) == HAL_OK)
    {
        /* Block until TC fires or timeout (guards against hardware faults). */
        xSemaphoreTake(s_tx_done, pdMS_TO_TICKS(UART_DMA_TIMEOUT_MS));
    }

    xSemaphoreGive(s_tx_mutex);
}

/* Overrides the HAL weak symbol — called from DMA TX-complete ISR. */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART3) return;
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(s_tx_done, &woken);
    portYIELD_FROM_ISR(woken);
}

#else /* CM7 stub — USART3 is owned by CM4 */

void bsp_uart_init(void) {}

void bsp_uart_transmit(const uint8_t *buf, uint16_t len)
{
    (void)buf;
    (void)len;
}

#endif
