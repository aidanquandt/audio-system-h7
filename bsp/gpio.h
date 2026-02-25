#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BSP_GPIO_LED_RED   = 0,  /* LD1 - PJ2  */
    BSP_GPIO_LED_GREEN = 1,  /* LD2 - PI13 */
    BSP_GPIO_COUNT
} bsp_gpio_t;

void bsp_gpio_set(bsp_gpio_t pin);
void bsp_gpio_reset(bsp_gpio_t pin);
void bsp_gpio_toggle(bsp_gpio_t pin);

#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */
