#include "led/led.h"
#include "rpc/generated/rpc_generated.h"
#include "bsp/gpio.h"

/* Handlers are core-gated because each LED is owned by one core.
   Routing is declared via led_init() — no dest coupling in the schema. */

#ifdef CORE_CM4
void rpc_handle_led_toggle_green(void)
{
    bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
void rpc_handle_led_toggle_red(void)
{
    bsp_gpio_toggle(BSP_GPIO_LED_RED);
}
#endif /* CORE_CM7 */

void led_init(void)
{
    rpc_register_led_toggle_green(DEST_CM4);
    rpc_register_led_toggle_red(DEST_CM7);
}
