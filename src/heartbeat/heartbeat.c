#include "heartbeat/heartbeat.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#endif

static void heartbeat_task(void *pvParameters)
{
    (void)pvParameters;

#ifdef CORE_CM4
    static const uint8_t msg[] = "CM4 heartbeat\r\n";
#endif

    for (;;) {
#ifdef CORE_CM4
        bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
        uart_transmit(msg, sizeof(msg) - 1);
#else
        bsp_gpio_toggle(BSP_GPIO_LED_RED);
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void heartbeat_init(void)
{
    xTaskCreate(heartbeat_task, "heartbeat", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
