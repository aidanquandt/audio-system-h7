#include "app_main.h"
#include "ipc.h"
#include "bsp/gpio.h"
#include "uart/uart.h"
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

static void uart_tx_task(void *pvParameters)
{
    (void)pvParameters;
    /* Must be in SRAM (not const/rodata/Flash) — DMA1 cannot reach Flash on H7. */
    static uint8_t msg[] = "CM4 heartbeat\r\n";

    for (;;) {
        uart_transmit(msg, sizeof(msg) - 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ipc_init();
    uart_init();
    xTaskCreate(led_task,     "LED",     256, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(uart_tx_task, "UART_TX", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
