#include "bsp/gpio/gpio.h"
#include "main.h"
#include <stdbool.h>

#define BSP_EXTI_LINES 16

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    bool          is_output;
} gpio_descriptor_t;

static const gpio_descriptor_t gpio_map[BSP_GPIO_COUNT] = {
#if defined(CORE_CM7)
    [BSP_GPIO_LED_RED]   = {LD6_GPIO_Port, LD6_Pin, true},
    [BSP_GPIO_LED_GREEN] = {NULL, 0, false},
    [BSP_GPIO_TOUCH_INT] = {NULL, 0, false},
#elif defined(CORE_CM4)
    [BSP_GPIO_LED_RED]   = {NULL, 0, false},
    [BSP_GPIO_LED_GREEN] = {LD7_GPIO_Port, LD7_Pin, true},
    [BSP_GPIO_TOUCH_INT] = {LCD_INT_GPIO_Port, LCD_INT_Pin, false},
#endif
};

static bsp_exti_callback_t exti_callbacks[BSP_EXTI_LINES];
static bsp_gpio_t          exti_line_to_bsp_pin[BSP_EXTI_LINES];

/*
 * Shadow state for each pin owned by this core (outputs only).
 * Avoids reading GPIOx->ODR in bsp_gpio_toggle(), which is unsafe on
 * STM32H7 dual-core: concurrent AHB4 reads from CM7 and CM4 can return
 * corrupted data (errata ES0396 2.2.9).  All writes go through BSRR
 * (via HAL_GPIO_WritePin) which is a pure write — no read required.
 */
static uint8_t shadow_state[BSP_GPIO_COUNT];

static inline bool is_owned(bsp_gpio_t pin)
{
    return pin < BSP_GPIO_COUNT && gpio_map[pin].port != NULL;
}

static inline bool is_output(bsp_gpio_t pin)
{
    return is_owned(pin) && gpio_map[pin].is_output;
}

void bsp_gpio_init(void)
{
    for (bsp_gpio_t pin = 0; pin < BSP_GPIO_COUNT; pin++)
    {
        if (is_output(pin))
        {
            bsp_gpio_reset(pin);
        }
    }
}

void bsp_gpio_set(bsp_gpio_t pin)
{
    if (!is_output(pin))
    {
        return;
    }
    shadow_state[pin] = 1;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_SET);
}

void bsp_gpio_reset(bsp_gpio_t pin)
{
    if (!is_output(pin))
    {
        return;
    }
    shadow_state[pin] = 0;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_RESET);
}

void bsp_gpio_toggle(bsp_gpio_t pin)
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
        return BSP_EXTI_LINES;
    }
    uint32_t line = 0;
    while ((gpio_pin & 1) == 0 && line < 16)
    {
        gpio_pin >>= 1;
        line++;
    }
    return (gpio_pin == 1) ? line : BSP_EXTI_LINES;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t line = pin_to_line(GPIO_Pin);
    if (line < BSP_EXTI_LINES && exti_callbacks[line] != NULL)
    {
        bsp_gpio_t bsp_pin = exti_line_to_bsp_pin[line];
        if (bsp_pin < BSP_GPIO_COUNT)
        {
            exti_callbacks[line](bsp_pin);
        }
    }
}

bool bsp_exti_register(bsp_gpio_t pin, bsp_exti_callback_t callback)
{
    if (!is_owned(pin) || gpio_map[pin].is_output)
    {
        return false;
    }
    uint16_t gpio_pin = gpio_map[pin].pin;
    uint32_t line     = pin_to_line(gpio_pin);
    if (line >= BSP_EXTI_LINES)
    {
        return false;
    }

    if (callback == NULL)
    {
        exti_callbacks[line]       = NULL;
        exti_line_to_bsp_pin[line] = BSP_GPIO_COUNT;
        return true;
    }

    exti_line_to_bsp_pin[line] = pin;
    exti_callbacks[line]       = callback;
    return true;
}
