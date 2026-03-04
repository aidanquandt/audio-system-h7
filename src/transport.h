#pragma once

#include <stdint.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "stream_buffer.h"

typedef struct {
    void (*send)(const uint8_t *buf, size_t len);
    StreamBufferHandle_t (*get_rx_stream)(void);
} transport_t;

/** Initialise transport layer and register built-in transports (e.g. UART). Call after uart_init() on CM4. */
void               transport_init(void);
void               transport_register(const transport_t *t);
void               transport_send(const uint8_t *buf, size_t len);
size_t             transport_count(void);
const transport_t *transport_get(size_t idx);
