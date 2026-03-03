#pragma once

/* Payload capacity of a single RPC frame (bytes).
 * Must fit in a uint8_t length field. Fits comfortably in a COBS-encoded
 * UART frame and keeps rpc_frame_t well under a cache line. */
#define RPC_FRAME_MAX_PAYLOAD 32U

/* Depth of each inter-core RPC queue in SRAM3 — must be a power of 2. */
#define RPC_QUEUE_DEPTH 8U
