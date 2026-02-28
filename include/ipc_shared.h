#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef __DMB
#define __DMB()  __asm__ volatile ("dmb" ::: "memory")  /* CMSIS fallback for host/unit-test builds */
#endif

#define IPC_SRAM3_BASE    0x30040000UL
#define IPC_READY_FLAG    0xA5B6C7D8UL
#define IPC_VERSION       1U
#define IPC_QUEUE_DEPTH   8U    /* must be power of 2 */

/* HSEM channel assignments for IPC — part of the inter-core protocol. */
#define HSEM_CH_CM4_TO_CM7  0U
#define HSEM_CH_CM7_TO_CM4  1U

typedef enum {
    IPC_CMD_PING       = 0x01,
    IPC_CMD_PONG       = 0x02,
    IPC_CMD_SET_GAIN   = 0x10,
    IPC_CMD_SET_EQ     = 0x11,
    IPC_CMD_MUTE       = 0x12,
    IPC_CMD_PEAK_METER = 0x80,
    IPC_CMD_DSP_LOAD   = 0x81,
} ipc_cmd_t;

typedef struct {
    ipc_cmd_t cmd;
    uint32_t  args[3];
} ipc_msg_t;

typedef struct {
    volatile uint32_t  head;
    volatile uint32_t  tail;
    volatile ipc_msg_t slots[IPC_QUEUE_DEPTH];
} ipc_queue_t;

typedef struct {
    volatile uint32_t ready_flag;
    volatile uint32_t version;
    uint32_t          _pad[6];  /* pad cm4_to_cm7 to offset 32 — enforced by static assert below */
    ipc_queue_t       cm4_to_cm7;
    ipc_queue_t       cm7_to_cm4;
} ipc_shared_t;

#define IPC_SHARED  ((ipc_shared_t *)IPC_SRAM3_BASE)

_Static_assert((IPC_QUEUE_DEPTH & (IPC_QUEUE_DEPTH - 1U)) == 0U, "IPC_QUEUE_DEPTH must be power of 2");
_Static_assert(offsetof(ipc_shared_t, cm4_to_cm7) == 32U, "ipc_shared_t layout changed");
_Static_assert(sizeof(ipc_shared_t) <= 32768U, "ipc_shared_t exceeds SRAM3");
