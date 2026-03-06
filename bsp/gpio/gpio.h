#pragma once

#include <stdbool.h>

typedef enum {
    BSP_GPIO_LED_RED   = 0, /* LD6 (CM7) / LD7 (CM4) */
    BSP_GPIO_LED_GREEN = 1,
    BSP_GPIO_TOUCH_INT = 2, /* PG2 - LCD_INT, touch data ready (CM4 only) */
    BSP_GPIO_COUNT
} bsp_gpio_t;

typedef void (*bsp_exti_callback_t)(bsp_gpio_t pin);

void bsp_gpio_init(void);
void bsp_gpio_set(bsp_gpio_t pin);
void bsp_gpio_reset(bsp_gpio_t pin);
void bsp_gpio_toggle(bsp_gpio_t pin);
bool bsp_exti_register(bsp_gpio_t pin, bsp_exti_callback_t callback);
