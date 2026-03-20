#include "src/app_main/app_main.h"
#include "FreeRTOS.h"
#include "drivers/gpio/gpio_driver.h"
#include "drivers/hsem/hsem_driver.h"
#include "drivers/lcd/lcd_driver.h"
#include "drivers/sdram/sdram_driver.h"
#include "drivers/touch/touch_driver.h"
#include "drivers/uart/uart_driver.h"
#include "src/display/display.h"
#include "src/dlog/dlog.h"
#include "src/heartbeat/heartbeat.h"
#include "src/led/led.h"
#include "src/messaging_protocol/messaging_protocol.h"
#include "src/touch/touch.h"
#include "task.h"

void app_main(void)
{
    gpio_driver_init();
    hsem_driver_init();
    sdram_driver_init();
    lcd_driver_init();
    uart_driver_init();
    touch_driver_init();

    messaging_protocol_transport_t uart_transport;
    messaging_protocol_uart_transport_init(&uart_transport);
    messaging_protocol_init(&uart_transport);

    display_init();
    led_init();
    heartbeat_init();
    dlog_init();
    touch_init();

    vTaskDelete(NULL);
}
