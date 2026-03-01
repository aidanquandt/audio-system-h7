#pragma once

#include <stdint.h>
#include <stdbool.h>

void bsp_uart_set_tx_cplt_cb(void (*cb)(void));
bool bsp_uart_transmit_dma(const uint8_t *buf, uint16_t len);