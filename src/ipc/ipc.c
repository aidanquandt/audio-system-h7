#include "ipc/ipc.h"
#include "bsp/hsem.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Give ipc_on_message() enough headroom on top of the task's own usage. */
#define IPC_TASK_STACK_WORDS      512U
#define IPC_CM7_BOOT_TIMEOUT_MS   5000U

typedef struct {
    SemaphoreHandle_t       wakeup;
    volatile ipc_queue_t   *tx_queue;
    volatile ipc_queue_t   *rx_queue;
    uint32_t                tx_channel;
    uint32_t                rx_channel;
} ipc_ctx_t;

static ipc_ctx_t ipc_ctx;

static int ipc_queue_push(volatile ipc_queue_t *q, const ipc_msg_t *m)
{
    uint32_t head = q->head;
    uint32_t next = (head + 1U) & (IPC_QUEUE_DEPTH - 1U);
    if (next == q->tail) return -1;
    q->slots[head] = *m;
    __DMB();
    q->head = next;
    return 0;
}

static int ipc_queue_pop(volatile ipc_queue_t *q, ipc_msg_t *m)
{
    uint32_t tail = q->tail;
    if (tail == q->head) return -1;
    *m = q->slots[tail];
    __DMB();
    q->tail = (tail + 1U) & (IPC_QUEUE_DEPTH - 1U);
    return 0;
}

__attribute__((weak)) void ipc_on_message(const ipc_msg_t *msg) { (void)msg; }

static void on_hsem(uint32_t sem_mask)
{
    BaseType_t woken = pdFALSE;

    if (sem_mask & (1U << ipc_ctx.rx_channel)) {
        xSemaphoreGiveFromISR(ipc_ctx.wakeup, &woken);
    }

    portYIELD_FROM_ISR(woken);
}

static void ipc_task(void *arg)
{
    (void)arg;
    ipc_msg_t msg;

#ifndef CORE_CM7
    uint32_t boot_wait = 0;
    while (IPC_SHARED->ready_flag != IPC_READY_FLAG) {
        vTaskDelay(pdMS_TO_TICKS(1));
        if (++boot_wait >= IPC_CM7_BOOT_TIMEOUT_MS) {
            /* TODO: fault — CM7 did not initialize shared memory within 5 s */
            break;
        }
    }
    __DMB();
    if (IPC_SHARED->version != IPC_VERSION) {
        /* TODO: fault — IPC version mismatch between CM7 and CM4 images */
    }
#endif

    volatile ipc_queue_t *rx = ipc_ctx.rx_queue;
    bsp_hsem_register_callback(1U << ipc_ctx.rx_channel, on_hsem);
    bsp_hsem_arm(1U << ipc_ctx.rx_channel);

    for (;;) {
        xSemaphoreTake(ipc_ctx.wakeup, pdMS_TO_TICKS(50));
        while (ipc_queue_pop(rx, &msg) == 0) {
            ipc_on_message(&msg);
        }
    }
}

int ipc_send(ipc_cmd_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
    ipc_msg_t msg = {
        .cmd  = cmd,
        .args = { arg0, arg1, arg2 },
    };
    int rc;

    taskENTER_CRITICAL();
    rc = ipc_queue_push(ipc_ctx.tx_queue, &msg);
    taskEXIT_CRITICAL();

    if (rc != 0) {
        return rc;
    }

    bsp_hsem_notify(ipc_ctx.tx_channel);
    return 0;
}

void ipc_init(void)
{
    /* Idempotent — safe to call more than once (e.g. in unit tests). */
    if (ipc_ctx.wakeup != NULL) {
        return;
    }

    ipc_ctx.wakeup = xSemaphoreCreateBinary();
    if (ipc_ctx.wakeup == NULL) {
        /* TODO: fault — FreeRTOS heap exhausted during IPC semaphore creation */
        return;
    }

    bsp_hsem_init();

#ifdef CORE_CM7
    ipc_ctx.tx_queue   = &IPC_SHARED->cm7_to_cm4;
    ipc_ctx.rx_queue   = &IPC_SHARED->cm4_to_cm7;
    ipc_ctx.tx_channel = HSEM_CH_CM7_TO_CM4;
    ipc_ctx.rx_channel = HSEM_CH_CM4_TO_CM7;

    ipc_shared_t *sh = IPC_SHARED;
    sh->cm4_to_cm7.head = 0;
    sh->cm4_to_cm7.tail = 0;
    sh->cm7_to_cm4.head = 0;
    sh->cm7_to_cm4.tail = 0;
    sh->version = IPC_VERSION;
    __DMB();
    sh->ready_flag = IPC_READY_FLAG;
#else
    ipc_ctx.tx_queue   = &IPC_SHARED->cm4_to_cm7;
    ipc_ctx.rx_queue   = &IPC_SHARED->cm7_to_cm4;
    ipc_ctx.tx_channel = HSEM_CH_CM4_TO_CM7;
    ipc_ctx.rx_channel = HSEM_CH_CM7_TO_CM4;
#endif

    xTaskCreate(ipc_task, "ipc", IPC_TASK_STACK_WORDS, NULL, tskIDLE_PRIORITY + 2, NULL);
}
