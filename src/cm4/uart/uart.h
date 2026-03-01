#pragma once
#include <stdint.h>

void uart_init(void);
void uart_transmit(const uint8_t *buf, uint16_t len);