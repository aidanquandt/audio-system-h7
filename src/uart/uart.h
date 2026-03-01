#pragma once
#include <stdint.h>

/* Maximum length of a single message — longer payloads are truncated. */
#define UART_TX_MAX_MSG_LEN  128U

/* Queue depth: number of messages that can be pending before uart_transmit blocks. */
#define UART_TX_QUEUE_DEPTH  8U

void uart_init(void);
void uart_transmit(const uint8_t *buf, uint16_t len);
uint32_t uart_get_drop_count(void);
