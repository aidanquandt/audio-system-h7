#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct messaging_protocol_transport {
    /* Send buf[0..len-1]. Returns 0 on success, negative on error. */
    int (*send)(void *ctx, const uint8_t *buf, size_t len);

    /* Receive up to max_len bytes into buf. Blocks up to timeout_ms.
     * Returns number of bytes received (0 on timeout, no data). */
    size_t (*recv)(void *ctx, uint8_t *buf, size_t max_len, uint32_t timeout_ms);

    void *ctx;
} messaging_protocol_transport_t;
