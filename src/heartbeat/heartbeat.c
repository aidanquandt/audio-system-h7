#include "heartbeat/heartbeat.h"
#include "bsp/gpio.h"
#include "rpc/rpc.h"
#include "rpc/messages.h"
#include "FreeRTOS.h"
#include "task.h"

static void heartbeat_task(void *pvParameters)
{
    (void)pvParameters;
    static uint32_t seq = 0;

    for (;;) {
#ifdef CORE_CM4
        bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
        heartbeat_cm4_t hb = { .seq = seq++ };
        rpc_transmit(MSG_HEARTBEAT_CM4, &hb, sizeof(hb));
#else
        bsp_gpio_toggle(BSP_GPIO_LED_RED);
        heartbeat_cm7_t hb = { .seq = seq++ };
        rpc_transmit(MSG_HEARTBEAT_CM7, &hb, sizeof(hb));
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void heartbeat_init(void)
{
    xTaskCreate(heartbeat_task, "heartbeat", 384, NULL, tskIDLE_PRIORITY + 1, NULL);
}
