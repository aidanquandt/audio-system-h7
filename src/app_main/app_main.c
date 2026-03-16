#include "src/app_main/app_main.h"
#include "drivers/gpio/gpio_driver.h"
#include "drivers/sdram/sdram_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "src/dlog/dlog.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "drivers/lcd/lcd_driver.h"
#include "src/display/display.h"
#include "src/touch/touch.h"
#include "drivers/uart/uart_driver.h"
#include "drivers/touch/touch_driver.h"
#include "drivers/hsem/hsem_driver.h"
#include "src/transport/transport.h"

void app_main(void)
{
    gpio_driver_init();
    hsem_driver_init();
    sdram_driver_init();
    lcd_driver_init();
    uart_driver_init();
    touch_driver_init();

    display_init();
    transport_init();
    led_init();
    heartbeat_init();
    dlog_init();
    touch_init();

    vTaskDelete(NULL);
}
