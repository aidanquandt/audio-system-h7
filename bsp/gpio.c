#include "bsp/gpio.h"
#include "main.h"
#include <stdbool.h>

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} gpio_descriptor_t;

static const gpio_descriptor_t gpio_map[BSP_GPIO_COUNT] = {
#if defined(CORE_CM7)
    [BSP_GPIO_LED_RED]   = { LD6_GPIO_Port, LD6_Pin },
#elif defined(CORE_CM4)
    [BSP_GPIO_LED_GREEN] = { LD7_GPIO_Port, LD7_Pin },
#endif
};

/*
 * Shadow state for each pin owned by this core.
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

void bsp_gpio_set(bsp_gpio_t pin)
{
    if (!is_owned(pin)) return;
    shadow_state[pin] = 1;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_SET);
}

void bsp_gpio_reset(bsp_gpio_t pin)
{
    if (!is_owned(pin)) return;
    shadow_state[pin] = 0;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_RESET);
}

void bsp_gpio_toggle(bsp_gpio_t pin)
{
    if (!is_owned(pin)) return;
    shadow_state[pin] ^= 1;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin,
                      shadow_state[pin] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
