#include "app_main.h"
#include "ipc.h"
#include "FreeRTOS.h"
#include "task.h"

static void default_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ipc_init();
    xTaskCreate(default_task, "CM7_Default", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
