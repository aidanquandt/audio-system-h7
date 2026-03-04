#include "src/app_main.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat.h"
#include "src/led.h"
#include "src/rpc.h"

#ifdef CORE_CM4
#include "src/display.h"
#include "src/sdram.h"
#include "drivers/uart.h"
#include "src/transport.h"
#endif /* CORE_CM4 */

void app_main(void)
{
    bsp_gpio_init();

#ifdef CORE_CM4
    sdram_init();
    /* Display uses SDRAM framebuffer; init order must stay after sdram_init(). */
    display_init();
    display_test();
    uart_init();
    transport_init();
#endif
    rpc_init();
    led_init();
    heartbeat_init();
}
