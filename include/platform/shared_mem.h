#pragma once

#include <stdint.h>
#include "../protocol/rpc_types.h"

#ifndef __DMB
#define __DMB()                                                                                    \
    __asm__ volatile("dmb" ::                                                                      \
                         : "memory") /* CMSIS fallback for host/unit-test builds                   \
                                      */
#endif

#define SRAM3_BASE       0x30040000UL
#define SHMEM_READY_FLAG 0xA5B6C7D8UL
#define SHMEM_VERSION    1U

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

_Static_assert(sizeof(shared_mem_t) <= 32768U, "shared_mem_t exceeds SRAM3");
