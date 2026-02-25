#include "app_main.h"
#include "FreeRTOS.h"
#include "stm32h7xx_hal.h"
#include "task.h"
#include "main.h"

static void led_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);  // Red LED
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // Green LED
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    xTaskCreate(led_task, "LED", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
