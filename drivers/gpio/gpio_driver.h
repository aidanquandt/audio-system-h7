#pragma once

#include <stdbool.h>

typedef enum {
    GPIO_DRIVER_LED_RED   = 0, /* LD6 (CM7) / LD7 (CM4) */
    GPIO_DRIVER_LED_GREEN = 1,
    GPIO_DRIVER_TOUCH_INT = 2, /* PG2 - LCD_INT, touch data ready (CM4 only) */
    GPIO_DRIVER_COUNT
} gpio_driver_pin_t;

typedef void (*gpio_driver_exti_callback_t)(gpio_driver_pin_t pin);

void gpio_driver_init(void);
void gpio_driver_set(gpio_driver_pin_t pin);
void gpio_driver_reset(gpio_driver_pin_t pin);
void gpio_driver_toggle(gpio_driver_pin_t pin);
bool gpio_driver_exti_register(gpio_driver_pin_t pin, gpio_driver_exti_callback_t callback);
