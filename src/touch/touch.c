#include "src/touch/touch.h"

#ifdef CORE_CM4

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "drivers/gpio/gpio.h"
#include "drivers/touch/touch.h"
#include "drivers/lcd/lcd.h"

#define TOUCH_DOT_SIZE        12U
#define TOUCH_TASK_STACK_WORDS 256U
#define RGB565_WHITE           0xFFFFU

static SemaphoreHandle_t s_touch_sem;

/* Called from ISR when touch INT (PG2) fires. Keep minimal: only unblock the task. */
static void touch_exti_cb(gpio_driver_pin_t pin)
{
    (void)pin;
    if (s_touch_sem == NULL)
    {
        return;
    }
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(s_touch_sem, &woken);
    portYIELD_FROM_ISR(woken);
}

static void touch_task(void *pvParameters)
{
    (void)pvParameters;
    touch_state_t state;
    for (;;)
    {
        if (xSemaphoreTake(s_touch_sem, portMAX_DELAY) == pdTRUE)
        {
            if (touch_driver_read(&state))
            {
                if (state.pressed)
                {
                    uint16_t x0 = state.x >= TOUCH_DOT_SIZE / 2 ? state.x - TOUCH_DOT_SIZE / 2 : 0;
                    uint16_t y0 = state.y >= TOUCH_DOT_SIZE / 2 ? state.y - TOUCH_DOT_SIZE / 2 : 0;
                    lcd_driver_fill_rect_sync(x0, y0, TOUCH_DOT_SIZE, TOUCH_DOT_SIZE, RGB565_WHITE);
                }
            }
        }
    }
}

void touch_init(void)
{
    s_touch_sem = xSemaphoreCreateBinary();
    if (s_touch_sem == NULL)
    {
        return;
    }
    if (!gpio_driver_exti_register(GPIO_DRIVER_TOUCH_INT, touch_exti_cb))
    {
        vSemaphoreDelete(s_touch_sem);
        s_touch_sem = NULL;
        return;
    }
    xTaskCreate(touch_task, "touch", TOUCH_TASK_STACK_WORDS, NULL, 1, NULL);
}

#else

void touch_init(void)
{
}

#endif /* CORE_CM4 */
