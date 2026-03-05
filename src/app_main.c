#include "src/app_main.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat.h"
#include "src/led.h"
#include "src/rpc.h"
#include "src/display.h"
#include "src/sdram.h"
#include "drivers/uart.h"
#include "src/transport.h"

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
