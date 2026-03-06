#include "src/app_main/app_main.h"
#include "bsp/gpio/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "src/rpc/rpc.h"
#include "src/display/display.h"
#include "src/sdram/sdram.h"
#include "src/touch/touch.h"
#include "drivers/uart/uart.h"
#include "drivers/touch/touch.h" /* touch_driver_init */
#include "src/transport/transport.h"

void app_main(void)
{
    bsp_gpio_init();
    sdram_init();
    /* Display uses SDRAM framebuffer; init order must stay after sdram_init(). */
    display_init();
    uart_driver_init();
    transport_init();
    rpc_init();
    led_init();
    heartbeat_init();

    if (touch_driver_init())
    {
        touch_init();
    }
}
