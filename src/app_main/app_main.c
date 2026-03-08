#include "src/app_main/app_main.h"
#include "drivers/gpio/gpio.h"
#include "drivers/sdram/sdram.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "drivers/lcd/lcd.h"
#include "src/rpc/rpc.h"
#include "src/display/display.h"
#include "src/touch/touch.h"
#include "drivers/uart/uart.h"
#include "drivers/touch/touch.h" /* touch_driver_init */
#include "drivers/hsem/hsem.h"
#include "src/transport/transport.h"

void app_main(void)
{
    gpio_driver_init();
    hsem_driver_init();
    sdram_driver_init();
    lcd_driver_init();
    display_init();
    uart_driver_init();
    transport_init();
    rpc_init();
    led_init();
    heartbeat_init();
    touch_driver_init();
    touch_init();
}
