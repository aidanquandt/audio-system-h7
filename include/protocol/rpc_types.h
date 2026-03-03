#pragma once

#include <stdint.h>

/* ── RPC inter-core frame format ─────────────────────────────────────────── *
 *
 * These types define the wire format shared between CM4 and CM7 via SRAM3.
 * Both cores must agree on this layout; it is part of the inter-core
 * protocol, not an implementation detail of src/rpc/. */

/* Payload capacity of one RPC frame — must fit in a uint8_t length field. */
#define RPC_FRAME_MAX_PAYLOAD  32U

/* Depth of each inter-core RPC queue in SRAM3 — must be a power of 2. */
#define RPC_QUEUE_DEPTH         8U

typedef struct __attribute__((aligned(4))) {
    uint8_t msg_id;
    uint8_t len;
    uint8_t _pad[2]; /* keep slots[] word-aligned in the array */
    uint8_t data[RPC_FRAME_MAX_PAYLOAD];
} rpc_frame_t;

typedef struct {
    volatile uint32_t    head;
    volatile uint32_t    tail;
    volatile rpc_frame_t slots[RPC_QUEUE_DEPTH];
} rpc_frame_queue_t;

_Static_assert((RPC_QUEUE_DEPTH & (RPC_QUEUE_DEPTH - 1U)) == 0U,
               "RPC_QUEUE_DEPTH must be a power of 2");
_Static_assert(sizeof(rpc_frame_t) % 4U == 0U,
               "rpc_frame_t size must be a multiple of 4");
