#include "src/heartbeat/heartbeat.h"
#include "FreeRTOS.h"
#include "drivers/gpio/gpio_driver.h"
#include "task.h"

static void heartbeat_task(void* pvParameters)
{
    (void) pvParameters;

    for (;;)
    {
#ifdef CORE_CM4
        gpio_driver_toggle(GPIO_DRIVER_LED_GREEN);
#else
        gpio_driver_toggle(GPIO_DRIVER_LED_RED);
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void heartbeat_init(void)
{
    xTaskCreate(heartbeat_task, "heartbeat", 384, NULL, tskIDLE_PRIORITY + 1, NULL);
}
