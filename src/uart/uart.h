#pragma once
#include <stdint.h>

#define UART_TX_MAX_MSG_LEN  128U
#define UART_TX_QUEUE_DEPTH  8U
#define UART_RX_BUF_LEN      128U

void uart_init(void);
void uart_transmit(const uint8_t *buf, uint16_t len);
uint32_t uart_get_drop_count(void);

/* Override to handle received data. Called from task context. */
void uart_on_receive(const uint8_t *buf, uint16_t len);
