#pragma once

#include <stdint.h>
#include <stddef.h>
#include "rpc/rpc_config.h"

#ifndef __DMB
#define __DMB()                                                                                    \
    __asm__ volatile("dmb" ::                                                                      \
                         : "memory") /* CMSIS fallback for host/unit-test builds                   \
                                      */
#endif

#define SRAM3_BASE       0x30040000UL
#define SHMEM_READY_FLAG 0xA5B6C7D8UL
#define SHMEM_VERSION    1U

/* HSEM channel assignments — part of the inter-core wire protocol.
 * CM7 writes ready_flag; CM4 polls it before arming these channels. */
#define HSEM_CH_RPC_CM4_TO_CM7 0U
#define HSEM_CH_RPC_CM7_TO_CM4 1U

/* ── RPC inter-core queue types ──────────────────────────────────────────── */

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

/* ── Shared SRAM3 layout @ SRAM3_BASE ────────────────────────────────────── *
 *
 * All fields placed here are visible to both CM4 and CM7 simultaneously.
 * CM7 owns initialisation: it zeroes all queues, writes version, then writes
 * ready_flag last as a DMB release barrier that CM4 polls before use.
 *
 * Future additions (e.g. audio DMA double-buffers) go here. */

typedef struct {
    volatile uint32_t ready_flag;     /* written last by CM7 as release barrier */
    volatile uint32_t version;        /* SHMEM_VERSION — checked by CM4 on boot  */
    rpc_frame_queue_t cm4_to_cm7_rpc; /* CM4 pushes, CM7 pops                    */
    rpc_frame_queue_t cm7_to_cm4_rpc; /* CM7 pushes, CM4 pops + forwards to wire */
} shared_mem_t;

#define SHARED_MEM ((shared_mem_t *)SRAM3_BASE)

_Static_assert((RPC_QUEUE_DEPTH & (RPC_QUEUE_DEPTH - 1U)) == 0U,
               "RPC_QUEUE_DEPTH must be power of 2");
_Static_assert(sizeof(rpc_frame_t) % 4U == 0U, "rpc_frame_t size must be a multiple of 4");
_Static_assert(sizeof(shared_mem_t) <= 32768U, "shared_mem_t exceeds SRAM3");
