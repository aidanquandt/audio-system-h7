#pragma once
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include <stdint.h>

#define UART_TX_MAX_MSG_LEN 1028U /* 1024 max frame + 2 length prefix + 2 margin */
#define UART_TX_QUEUE_DEPTH 8U
#define UART_RX_BUF_LEN     128U
#define UART_RX_STREAM_SIZE 2048U /* Enough to buffer 2 max-size frames */

void uart_driver_init(void);
void uart_driver_transmit(const uint8_t* buf, size_t len);
uint32_t uart_driver_get_drop_count(void);

/* Protocol layer reads raw bytes from here. Blocks until data is available. */
StreamBufferHandle_t uart_driver_get_rx_stream(void);
