#include "app_main/app_main.h"
#include "bsp/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "heartbeat/heartbeat.h"
#include "led/led.h"
#include "rpc/rpc.h"

#ifdef CORE_CM4
#include "display/display.h"
#include "sdram/sdram.h"
#include "uart/uart.h"
#include "transport/transport.h"

static const transport_t uart_transport = {
    .send          = uart_transmit,
    .get_rx_stream = uart_get_rx_stream,
};
#endif /* CORE_CM4 */

void app_main(void)
{
    bsp_gpio_init();

#ifdef CORE_CM4
    if (!sdram_init())
    {
        for (;;)
        {
            bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
            vTaskDelay(pdMS_TO_TICKS(150));
        }
    }
    display_init();
    display_test();
    uart_init();
    transport_register(&uart_transport);
#endif
    rpc_init();
    led_init();
    heartbeat_init();
}
