#include "bsp/gpio.h"
#include "main.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} gpio_descriptor_t;

static const gpio_descriptor_t gpio_map[BSP_GPIO_COUNT] = {
    [BSP_GPIO_LED_RED]   = { LD1_GPIO_Port, LD1_Pin },  /* PJ2  */
    [BSP_GPIO_LED_GREEN] = { LD2_GPIO_Port, LD2_Pin },  /* PI13 */
};

void bsp_gpio_set(bsp_gpio_t pin)
{
    if (pin >= BSP_GPIO_COUNT) return;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_SET);
}

void bsp_gpio_reset(bsp_gpio_t pin)
{
    if (pin >= BSP_GPIO_COUNT) return;
    HAL_GPIO_WritePin(gpio_map[pin].port, gpio_map[pin].pin, GPIO_PIN_RESET);
}

void bsp_gpio_toggle(bsp_gpio_t pin)
{
    if (pin >= BSP_GPIO_COUNT) return;
    HAL_GPIO_TogglePin(gpio_map[pin].port, gpio_map[pin].pin);
}
