#include "ipc.h"
#include "bsp/hsem.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t s_wakeup;

__attribute__((weak)) void ipc_on_message(const ipc_msg_t *msg) { (void)msg; }

static void on_hsem(uint32_t sem_mask)
{
    BaseType_t woken = pdFALSE;

#ifdef CORE_CM7
    if (sem_mask & (1U << HSEM_CH_CM4_TO_CM7)) {
        xSemaphoreGiveFromISR(s_wakeup, &woken);
    }
#else
    if (sem_mask & (1U << HSEM_CH_CM7_TO_CM4)) {
        xSemaphoreGiveFromISR(s_wakeup, &woken);
    }
#endif

    portYIELD_FROM_ISR(woken);
}

static void ipc_dispatch(const ipc_msg_t *msg)
{
    ipc_on_message(msg);
}

static void ipc_task(void *arg)
{
    (void)arg;
    ipc_msg_t msg;

#ifdef CORE_CM7
    volatile ipc_queue_t *rx = &IPC_SHARED->cm4_to_cm7;
    bsp_hsem_arm(1U << HSEM_CH_CM4_TO_CM7);
#else
    while (IPC_SHARED->ready_flag != IPC_READY_FLAG) {
        vTaskDelay(1);
    }
    volatile ipc_queue_t *rx = &IPC_SHARED->cm7_to_cm4;
    bsp_hsem_arm(1U << HSEM_CH_CM7_TO_CM4);
#endif

    for (;;) {
        xSemaphoreTake(s_wakeup, pdMS_TO_TICKS(50));
        while (ipc_queue_pop(rx, &msg) == 0) {
            ipc_dispatch(&msg);
        }
    }
}

void ipc_send(ipc_cmd_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
    ipc_msg_t msg = {
        .cmd    = (uint32_t)cmd,
        .args   = { arg0, arg1, arg2 },
    };

#ifdef CORE_CM7
    ipc_queue_push(&IPC_SHARED->cm7_to_cm4, &msg);
    bsp_hsem_notify(HSEM_CH_CM7_TO_CM4);
#else
    ipc_queue_push(&IPC_SHARED->cm4_to_cm7, &msg);
    bsp_hsem_notify(HSEM_CH_CM4_TO_CM7);
#endif
}

void ipc_init(void)
{
    s_wakeup = xSemaphoreCreateBinary();

    bsp_hsem_init();
    bsp_hsem_register_callback(on_hsem);

#ifdef CORE_CM7
    ipc_shared_t *sh = IPC_SHARED;
    sh->cm4_to_cm7.head = 0;
    sh->cm4_to_cm7.tail = 0;
    sh->cm7_to_cm4.head = 0;
    sh->cm7_to_cm4.tail = 0;
    sh->version = IPC_VERSION;
    __asm__ volatile ("dmb" ::: "memory");
    sh->ready_flag = IPC_READY_FLAG;
#endif

    xTaskCreate(ipc_task, "ipc", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}
