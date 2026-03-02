#include "bsp/uart.h"
#include <stdbool.h>

#ifdef CORE_CM4

#include "usart.h"

static void (*tx_cplt_cb)(void);
static void (*rx_cplt_cb)(void);

void bsp_uart_set_tx_cplt_cb(void (*cb)(void))
{
    tx_cplt_cb = cb;
}

void bsp_uart_set_rx_cplt_cb(void (*cb)(void))
{
    rx_cplt_cb = cb;
}

bool bsp_uart_transmit_dma(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return false;
    return HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buf, len) == HAL_OK;
}

bool bsp_uart_receive_dma(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return false;
    return HAL_UART_Receive_DMA(&huart3, buf, len) == HAL_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART3) return;
    if (tx_cplt_cb != NULL) tx_cplt_cb();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART3) return;
    if (rx_cplt_cb != NULL) rx_cplt_cb();
}

#else /* CM7 stub — USART3 is owned by CM4 */

void bsp_uart_set_tx_cplt_cb(void (*cb)(void)) { (void)cb; }
void bsp_uart_set_rx_cplt_cb(void (*cb)(void)) { (void)cb; }
bool bsp_uart_transmit_dma(const uint8_t *buf, uint16_t len) { (void)buf; (void)len; return true; }
bool bsp_uart_receive_dma(uint8_t *buf, uint16_t len)        { (void)buf; (void)len; return true; }

#endif
