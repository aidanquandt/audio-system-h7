#pragma once
#include <stdint.h>
#include "FreeRTOS.h"
#include "stream_buffer.h"

#define UART_TX_MAX_MSG_LEN  128U
#define UART_TX_QUEUE_DEPTH  8U
#define UART_RX_BUF_LEN      128U
#define UART_RX_STREAM_SIZE  512U

void uart_init(void);
void uart_transmit(const uint8_t *buf, uint16_t len);
uint32_t uart_get_drop_count(void);

/* Protocol layer reads raw bytes from here. Blocks until data is available. */
StreamBufferHandle_t uart_get_rx_stream(void);
