#include "src/heartbeat/heartbeat.h"
#include "drivers/gpio/gpio.h"
#include "generated/rpc.h"
#include "FreeRTOS.h"
#include "task.h"

static uint32_t s_seq = 0; /* module-level so it persists across any rescheduling */

static void heartbeat_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
#ifdef CORE_CM4
        gpio_driver_toggle(GPIO_DRIVER_LED_GREEN);
        heartbeat_cm4_t hb = {.seq = s_seq++};
        (void)rpc_transmit_heartbeat_cm4(&hb);
#else
        gpio_driver_toggle(GPIO_DRIVER_LED_RED);
        heartbeat_cm7_t hb = {.seq = s_seq++};
        (void)rpc_transmit_heartbeat_cm7(&hb);
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void heartbeat_init(void)
{
    xTaskCreate(heartbeat_task, "heartbeat", 384, NULL, tskIDLE_PRIORITY + 1, NULL);
}
