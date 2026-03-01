#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  Blocking UART transmit.
 *
 * Wraps HAL_UART_Transmit on USART3 (ST-Link VCP, 115200 8N1).
 * On CM7 this is a no-op stub — USART3 belongs to the CM4 domain.
 *
 * @param buf  Pointer to data buffer.
 * @param len  Number of bytes to send.
 */
void bsp_uart_transmit(const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
