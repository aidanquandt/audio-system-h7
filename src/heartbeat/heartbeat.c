#include "heartbeat/heartbeat.h"
#include "bsp/gpio.h"
#include "rpc/rpc.h"
#include "rpc/messages.h"
#include "FreeRTOS.h"
#include "task.h"

static void heartbeat_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
#ifdef CORE_CM4
        static uint32_t seq = 0;
        bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
        ping_t ping = { .seq = seq++ };
        rpc_transmit(MSG_PING, &ping, sizeof(ping));
#else
        bsp_gpio_toggle(BSP_GPIO_LED_RED);
        peak_meter_t meter = { .channel = 0, .peak_db = -20.0f };
        rpc_transmit(MSG_PEAK_METER, &meter, sizeof(meter));
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void heartbeat_init(void)
{
    xTaskCreate(heartbeat_task, "heartbeat", 384, NULL, tskIDLE_PRIORITY + 1, NULL);
}
