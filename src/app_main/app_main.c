#include "src/app_main/app_main.h"
#include "bsp/gpio/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "src/rpc/rpc.h"
#include "src/display/display.h"
#include "src/sdram/sdram.h"
#include "drivers/uart/uart.h"
#include "src/transport/transport.h"

void app_main(void)
{
    bsp_gpio_init();
    sdram_init();
    /* Display uses SDRAM framebuffer; init order must stay after sdram_init(). */
    display_init();
    uart_init();
    transport_init();
    rpc_init();
    led_init();
    heartbeat_init();

    for (int i = 0; i < 10; i++)
    {
        display_test();
    }
}
