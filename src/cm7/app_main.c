#include "app_main.h"
#include "FreeRTOS.h"
#include "task.h"

static void default_task(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;) {
        // Your CM7 application code here (DSP processing, etc.)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // Create your application tasks here
    xTaskCreate(default_task, "CM7_Default", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
