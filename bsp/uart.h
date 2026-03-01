#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  Initialise UART BSP resources.
 *
 * Must be called once before bsp_uart_transmit, before the FreeRTOS
 * scheduler starts.  Creates the DMA-completion semaphore.
 */
void bsp_uart_init(void);

/**
 * @brief  DMA-backed UART transmit (task-blocking).
 *
 * Launches HAL_UART_Transmit_DMA and suspends the calling task until
 * HAL_UART_TxCpltCallback signals completion.  The CPU is free to run
 * other tasks while bytes are clocking out.
 * On CM7 this is a no-op stub — USART3 belongs to the CM4 domain.
 *
 * @param buf  Pointer to data buffer (must remain valid until return).
 *             MUST reside in D2 SRAM — DMA1 on H7 cannot access Flash
 *             or DTCM.  Do NOT pass a pointer to a 'const' / rodata buffer.
 * @param len  Number of bytes to send.
 */
void bsp_uart_transmit(const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
