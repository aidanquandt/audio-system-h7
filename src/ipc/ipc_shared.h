#pragma once

#include <stdint.h>
#include <stddef.h>
#include "rpc/rpc_config.h"

#ifndef __DMB
#define __DMB()  __asm__ volatile ("dmb" ::: "memory")  /* CMSIS fallback for host/unit-test builds */
#endif

#define IPC_SRAM3_BASE   0x30040000UL
#define IPC_READY_FLAG   0xA5B6C7D8UL
#define IPC_VERSION      1U

/* HSEM channel assignments — part of the inter-core wire protocol.
 * CM7 writes ready_flag; CM4 polls it before arming these channels. */
#define HSEM_CH_RPC_CM4_TO_CM7  0U
#define HSEM_CH_RPC_CM7_TO_CM4  1U

/* ── RPC inter-core queue types ──────────────────────────────────────────── */

typedef struct __attribute__((aligned(4))) {
    uint8_t msg_id;
    uint8_t len;
    uint8_t _pad[2];                    /* keep slots[] word-aligned in the array */
    uint8_t data[RPC_FRAME_MAX_PAYLOAD];
} rpc_frame_t;

typedef struct {
    volatile uint32_t    head;
    volatile uint32_t    tail;
    volatile rpc_frame_t slots[RPC_QUEUE_DEPTH];
} rpc_frame_queue_t;

/* ── Shared memory layout @ IPC_SRAM3_BASE ───────────────────────────────── */

typedef struct {
    volatile uint32_t  ready_flag;      /* written last by CM7 as a release barrier */
    volatile uint32_t  version;         /* IPC_VERSION — checked by CM4 on boot     */
    rpc_frame_queue_t  cm4_to_cm7_rpc;  /* CM4 pushes, CM7 pops                     */
    rpc_frame_queue_t  cm7_to_cm4_rpc;  /* CM7 pushes, CM4 pops + forwards to wire  */
} ipc_shared_t;

#define IPC_SHARED  ((ipc_shared_t *)IPC_SRAM3_BASE)

_Static_assert((RPC_QUEUE_DEPTH & (RPC_QUEUE_DEPTH - 1U)) == 0U, "RPC_QUEUE_DEPTH must be power of 2");
_Static_assert(sizeof(rpc_frame_t) % 4U == 0U,                   "rpc_frame_t size must be a multiple of 4");
_Static_assert(sizeof(ipc_shared_t) <= 32768U,                    "ipc_shared_t exceeds SRAM3");
