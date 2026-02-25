#include "app_main.h"
#include "FreeRTOS.h"
#include "task.h"

static void default_task(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;) {
        // Your CM4 application code here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // Create your application tasks here
    xTaskCreate(default_task, "CM4_Default", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
