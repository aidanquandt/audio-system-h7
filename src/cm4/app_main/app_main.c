#include "app_main.h"
#include "ipc.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

static void led_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    ipc_init();
    xTaskCreate(led_task, "LED", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
