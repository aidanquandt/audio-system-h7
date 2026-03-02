#include "app_main/app_main.h"
#include "heartbeat/heartbeat.h"
#include "rpc/rpc.h"
#include "rpc/rpc_generated.h"
#include "bsp/gpio.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#include "transport/transport.h"

static const transport_t uart_transport = {
    .send          = uart_transmit,
    .get_rx_stream = uart_get_rx_stream,
};

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

void app_main(void)
{
#ifdef CORE_CM4
    uart_init();
    transport_register(&uart_transport);
#endif

    rpc_register_all();
    rpc_init();
    heartbeat_init();
}

