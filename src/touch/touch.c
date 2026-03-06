#include "src/touch/touch.h"

#ifdef CORE_CM4

#include "FreeRTOS.h"
#include "task.h"
#include "drivers/touch/touch.h"
#include "drivers/lcd/lcd.h"

#define TOUCH_DOT_SIZE 12U
#define RGB565_WHITE   0xFFFFU
#define TOUCH_POLL_MS  20

/* Currently we poll. The GT911 drives LCD_INT_Pin (PG2) when buffer is ready;
 * an interrupt-driven design would use EXTI on that pin, give a semaphore from
 * the ISR, and have this task block on the semaphore for lower latency and no
 * idle polling. See docs/CubeMX_I2C4_touch.md. */

static void touch_task(void *pvParameters)
{
    (void)pvParameters;
    touch_state_t state;
    for (;;)
    {
        if (touch_driver_get_last(&state))
        {
            if (state.pressed)
            {
                uint16_t x0 = state.x >= TOUCH_DOT_SIZE / 2 ? state.x - TOUCH_DOT_SIZE / 2 : 0;
                uint16_t y0 = state.y >= TOUCH_DOT_SIZE / 2 ? state.y - TOUCH_DOT_SIZE / 2 : 0;
                lcd_driver_fill_rect_sync(x0, y0, TOUCH_DOT_SIZE, TOUCH_DOT_SIZE, RGB565_WHITE);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(TOUCH_POLL_MS));
    }
}

void touch_init(void)
{
    xTaskCreate(touch_task, "touch", 128, NULL, 1, NULL);
}

#else

void touch_init(void)
{
    (void)0;
}

#endif /* CORE_CM4 */
