#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  Register a callback invoked from HAL_UART_TxCpltCallback (ISR context).
 *
 * The driver layer uses this to get notified when DMA TX is physically
 * complete.  At most one callback is supported.
 */
void bsp_uart_set_tx_cplt_cb(void (*cb)(void));

/**
 * @brief  Fire-and-forget DMA TX.  Returns true if DMA was accepted.
 *
 * No blocking, no RTOS knowledge.  Caller is responsible for
 * synchronisation.  Buffer MUST reside in D2 SRAM (not Flash / DTCM) —
 * DMA1 on H7 cannot access those regions.
 *
 * @param buf  Pointer to data buffer (valid until cb fires).
 * @param len  Number of bytes to send.
 * @return     true if HAL accepted the transfer, false otherwise.
 */
bool bsp_uart_transmit_dma(const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
