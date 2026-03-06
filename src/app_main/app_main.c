#include "src/app_main/app_main.h"
#include "bsp/gpio/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "src/rpc/rpc.h"
#include "src/display/display.h"
#include "src/sdram/sdram.h"
#include "drivers/uart/uart.h"
#include "drivers/touch/touch.h"
#include "drivers/lcd/lcd.h"
#include "src/transport/transport.h"

#define TOUCH_DOT_SIZE 12U
#define RGB565_WHITE   0xFFFFU
#define RGB565_BLACK   0x0000U

static void touch_test_task(void *pvParameters)
{
    (void)pvParameters;
    touch_state_t state;
    for (;;)
    {
        if (touch_get_last(&state))
        {
            if (state.pressed)
            {
                // lcd_fill_sync(RGB565_WHITE);
                uint16_t x0 = state.x >= TOUCH_DOT_SIZE / 2 ? state.x - TOUCH_DOT_SIZE / 2 : 0;
                uint16_t y0 = state.y >= TOUCH_DOT_SIZE / 2 ? state.y - TOUCH_DOT_SIZE / 2 : 0;
                lcd_fill_rect_sync(x0, y0, TOUCH_DOT_SIZE, TOUCH_DOT_SIZE, RGB565_WHITE);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    bsp_gpio_init();
    sdram_init();
    /* Display uses SDRAM framebuffer; init order must stay after sdram_init(). */
    display_init();
    uart_init();
    transport_init();
    rpc_init();
    led_init();
    heartbeat_init();

    /* Draw corner markers first: red=top-left, green=top-right, blue=bottom-left, white=bottom-right.
     * Confirm these match the physical panel so we know display coords are correct. */
    display_draw_edge_boxes();

    if (touch_init())
    {
        xTaskCreate(touch_test_task, "touch", 128, NULL, 1, NULL);
    }
}
