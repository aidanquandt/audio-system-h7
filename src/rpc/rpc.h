#pragma once

#include <stdint.h>
#include <stddef.h>
#include "rpc/messages.h"

typedef enum { DEST_CM4, DEST_CM7, DEST_BOTH } rpc_dest_t;
typedef void (*rpc_handler_fn)(uint8_t msg_id, const uint8_t *payload, size_t len);

/* Register a handler for msg_id. Use dest to control routing on CM4:
 *   DEST_CM4 — dispatch locally
 *   DEST_CM7 — forward to CM7 via IPC (fn may be NULL)
 * On CM7, dest is ignored; all dispatched messages call fn directly. */
void rpc_register(uint8_t msg_id, rpc_dest_t dest, rpc_handler_fn fn);

void     rpc_init(void);

/* Encode and send a message. Returns 0 on success, -1 if the IPC queue is
 * full (CM7 only — CM4 path blocks until the UART TX queue has space). */
int      rpc_transmit(uint8_t msg_id, const void *payload, size_t len);

/* Returns the number of COBS frames dropped due to wire-RX buffer overrun
 * (CM4 only). Useful for diagnosing baud-rate or DMA issues. */
uint32_t rpc_get_wire_overrun_count(void);
