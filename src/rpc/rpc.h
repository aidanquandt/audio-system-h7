#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum { DEST_CM4, DEST_CM7, DEST_BOTH } rpc_dest_t;
typedef void (*rpc_handler_fn)(uint8_t msg_id, const uint8_t *payload, size_t len);

/* Register a handler for msg_id.
 *
 * dest controls CM4 dispatch only — CM7 always calls fn directly because
 * IPC delivery already implies the message was meant for it:
 *   DEST_CM4  — call fn locally, do not forward
 *   DEST_CM7  — push to IPC queue for CM7, fn may be NULL
 *   DEST_BOTH — call fn locally AND forward to CM7
 */
void rpc_register(uint8_t msg_id, rpc_dest_t dest, rpc_handler_fn fn);

void     rpc_init(void);

/* Encode and send a message. Returns 0 on success, -1 if the IPC queue is
 * full (CM7 only — CM4 path blocks until the UART TX queue has space). */
int      rpc_transmit(uint8_t msg_id, const void *payload, size_t len);

/* Returns the number of COBS frames dropped due to wire-RX buffer overrun
 * (CM4 only). Useful for diagnosing baud-rate or DMA issues. */
uint32_t rpc_get_wire_overrun_count(void);
