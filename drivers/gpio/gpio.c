#include "drivers/gpio/gpio.h"
#include "main.h"
#include <stdbool.h>

#define GPIO_DRIVER_EXTI_LINES 16

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    bool          is_output;
} gpio_descriptor_t;

static const gpio_descriptor_t gpio_map[GPIO_DRIVER_COUNT] = {
#if defined(CORE_CM7)
    [GPIO_DRIVER_LED_RED]   = {LD6_GPIO_Port, LD6_Pin, true},
    [GPIO_DRIVER_LED_GREEN] = {NULL, 0, false},
    [GPIO_DRIVER_TOUCH_INT] = {NULL, 0, false},
#elif defined(CORE_CM4)
    [GPIO_DRIVER_LED_RED]   = {NULL, 0, false},
    [GPIO_DRIVER_LED_GREEN] = {LD7_GPIO_Port, LD7_Pin, true},
    [GPIO_DRIVER_TOUCH_INT] = {LCD_INT_GPIO_Port, LCD_INT_Pin, false},
#endif
};

static gpio_driver_exti_callback_t exti_callbacks[GPIO_DRIVER_EXTI_LINES];
static gpio_driver_pin_t          exti_line_to_pin[GPIO_DRIVER_EXTI_LINES];

/*
 * Shadow state for each pin owned by this core (outputs only).
 * Avoids reading GPIOx->ODR in gpio_driver_toggle(), which is unsafe on
 * STM32H7 dual-core: concurrent AHB4 reads from CM7 and CM4 can return
 * corrupted data (errata ES0396 2.2.9).  All writes go through BSRR
 * (via HAL_GPIO_WritePin) which is a pure write — no read required.
 */
static uint8_t shadow_state[GPIO_DRIVER_COUNT];

static inline bool is_owned(gpio_driver_pin_t pin)
{
    return pin < GPIO_DRIVER_COUNT && gpio_map[pin].port != NULL;
}

static inline bool is_output(gpio_driver_pin_t pin)
{
    return is_owned(pin) && gpio_map[pin].is_output;
}

void gpio_driver_init(void)
{
    for (gpio_driver_pin_t pin = 0; pin < GPIO_DRIVER_COUNT; pin++)
    {
        if (is_output(pin))
        {
            gpio_driver_reset(pin);
        }
    }
}

void gpio_driver_set(gpio_driver_pin_t pin)
{
    if (!is_output(pin))
    {
        return;
    }
    shadow_state[pin] = 1;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_SET);
}

void gpio_driver_reset(gpio_driver_pin_t pin)
{
    if (!is_output(pin))
    {
        return;
    }
    shadow_state[pin] = 0;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_RESET);
}

void gpio_driver_toggle(gpio_driver_pin_t pin)
{
    if (!is_output(pin))
    {
        return;
    }
    shadow_state[pin] ^= 1;
    HAL_GPIO_WritePin(gpio_map[pin].port,
                      gpio_map[pin].pin,
                      shadow_state[pin] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static uint32_t pin_to_line(uint16_t gpio_pin)
{
    if (gpio_pin == 0)
    {
        return GPIO_DRIVER_EXTI_LINES;
    }
    uint32_t line = 0;
    while ((gpio_pin & 1) == 0 && line < 16)
    {
        gpio_pin >>= 1;
        line++;
    }
    return (gpio_pin == 1) ? line : GPIO_DRIVER_EXTI_LINES;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t line = pin_to_line(GPIO_Pin);
    if (line < GPIO_DRIVER_EXTI_LINES && exti_callbacks[line] != NULL)
    {
        gpio_driver_pin_t driver_pin = exti_line_to_pin[line];
        if (driver_pin < GPIO_DRIVER_COUNT)
        {
            exti_callbacks[line](driver_pin);
        }
    }
}

bool gpio_driver_exti_register(gpio_driver_pin_t pin, gpio_driver_exti_callback_t callback)
{
    if (!is_owned(pin) || gpio_map[pin].is_output)
    {
        return false;
    }
    uint16_t gpio_pin = gpio_map[pin].pin;
    uint32_t line     = pin_to_line(gpio_pin);
    if (line >= GPIO_DRIVER_EXTI_LINES)
    {
        return false;
    }

    if (callback == NULL)
    {
        exti_callbacks[line]   = NULL;
        exti_line_to_pin[line] = GPIO_DRIVER_COUNT;
        return true;
    }

    exti_line_to_pin[line] = pin;
    exti_callbacks[line]   = callback;
    return true;
}
