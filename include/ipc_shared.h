#pragma once

#include <stdint.h>

#define IPC_SRAM3_BASE    0x30040000UL
#define IPC_READY_FLAG    0xA5B6C7D8UL
#define IPC_VERSION       1U
#define IPC_QUEUE_DEPTH   8U    /* must be power of 2 */

#define HSEM_CH_CM4_TO_CM7  0U
#define HSEM_CH_CM7_TO_CM4  1U
#define HSEM_CH_BOOT_SYNC   2U

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
    uint32_t cmd;
    uint32_t args[3];
} ipc_msg_t;

typedef struct {
    volatile uint32_t  head;
    volatile uint32_t  tail;
    volatile ipc_msg_t slots[IPC_QUEUE_DEPTH];
} ipc_queue_t;

typedef struct {
    volatile uint32_t ready_flag;
    volatile uint32_t version;
    uint32_t          _pad[6];
    ipc_queue_t       cm4_to_cm7;
    ipc_queue_t       cm7_to_cm4;
} ipc_shared_t;

#define IPC_SHARED  ((ipc_shared_t *)IPC_SRAM3_BASE)

static inline int ipc_queue_push(volatile ipc_queue_t *q, const ipc_msg_t *m)
{
    uint32_t head = q->head;
    uint32_t next = (head + 1U) & (IPC_QUEUE_DEPTH - 1U);
    if (next == q->tail) return -1;
    q->slots[head] = *m;
    __asm__ volatile ("dmb" ::: "memory");
    q->head = next;
    return 0;
}

static inline int ipc_queue_pop(volatile ipc_queue_t *q, ipc_msg_t *m)
{
    uint32_t tail = q->tail;
    if (tail == q->head) return -1;
    *m = q->slots[tail];
    __asm__ volatile ("dmb" ::: "memory");
    q->tail = (tail + 1U) & (IPC_QUEUE_DEPTH - 1U);
    return 0;
}
