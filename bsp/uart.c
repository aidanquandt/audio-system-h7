#include "bsp/uart.h"

#ifdef CORE_CM4

#include "usart.h"

#define UART_TX_TIMEOUT_MS  100U

void bsp_uart_transmit(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return;
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, len, UART_TX_TIMEOUT_MS);
}

#else /* CM7 stub — USART3 is owned by CM4 */

void bsp_uart_transmit(const uint8_t *buf, uint16_t len)
{
    (void)buf;
    (void)len;
}

#endif
