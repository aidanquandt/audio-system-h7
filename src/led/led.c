#include "src/led/led.h"
#include "drivers/gpio/gpio_driver.h"

void led_init(void)
{
    /* RPC-based remote LED control removed.
       New protocol or control path can call gpio_driver_toggle()
       directly or via a new abstraction when defined. */
    (void)0;
}
