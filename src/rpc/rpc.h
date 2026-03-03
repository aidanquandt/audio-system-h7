#pragma once

#include <stdint.h>
#include <stddef.h>

/* Routing destination for rpc_register().
 *
 * Only meaningful on CM4 — CM7 always calls fn directly because IPC
 * delivery already implies the message was addressed to it.  Use
 * DEST_LOCAL on CM7 to make the intent explicit at the call site. */
typedef enum {
    DEST_CM4   = 0, /* CM4 handles locally; do not forward to CM7        */
    DEST_CM7   = 1, /* CM4 forwards via IPC; CM7 handles (fn may be NULL) */
    DEST_BOTH  = 2, /* CM4 handles locally AND forwards to CM7            */
    DEST_LOCAL = 0, /* Alias: use on CM7 — routing is a no-op there       */
} rpc_dest_t;

typedef void (*rpc_handler_fn)(uint8_t msg_id, const uint8_t *payload, size_t len);

/* Register a handler for msg_id. */
void rpc_register(uint8_t msg_id, rpc_dest_t dest, rpc_handler_fn fn);

void rpc_init(void);

/* Encode and send a message.
 *
 * CM4: always returns 0 — UART TX is queued and never dropped.
 * CM7: returns 0 on success, -1 if the IPC queue to CM4 is full
 *      (caller may retry or drop at its discretion). */
int rpc_transmit(uint8_t msg_id, const void *payload, size_t len);

/* Returns the number of COBS frames dropped due to wire-RX buffer overrun
 * (CM4 only). Useful for diagnosing baud-rate or DMA issues. */
uint32_t rpc_get_wire_overrun_count(void);
