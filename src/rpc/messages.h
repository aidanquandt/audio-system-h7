#pragma once

#include <stdint.h>
#include "ipc/ipc_shared.h"

typedef enum {
    MSG_PING       = 0x01,
    MSG_PONG       = 0x02,
    MSG_SET_GAIN   = 0x10,
    MSG_PEAK_METER = 0x80,
} msg_id_t;

typedef enum {
    DEST_CM4,
    DEST_CM7,
} rpc_dest_t;

typedef struct __attribute__((packed)) { uint32_t seq; } ping_t;
typedef struct __attribute__((packed)) { uint32_t seq; } pong_t;
typedef struct __attribute__((packed)) { uint8_t channel; float gain_db; } set_gain_t;
typedef struct __attribute__((packed)) { uint8_t channel; float peak_db; } peak_meter_t;

_Static_assert(sizeof(ping_t)       <= RPC_FRAME_MAX_PAYLOAD, "ping_t exceeds RPC_FRAME_MAX_PAYLOAD");
_Static_assert(sizeof(pong_t)       <= RPC_FRAME_MAX_PAYLOAD, "pong_t exceeds RPC_FRAME_MAX_PAYLOAD");
_Static_assert(sizeof(set_gain_t)   <= RPC_FRAME_MAX_PAYLOAD, "set_gain_t exceeds RPC_FRAME_MAX_PAYLOAD");
_Static_assert(sizeof(peak_meter_t) <= RPC_FRAME_MAX_PAYLOAD, "peak_meter_t exceeds RPC_FRAME_MAX_PAYLOAD");
