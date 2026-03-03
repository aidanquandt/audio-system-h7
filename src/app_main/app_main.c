#include "app_main/app_main.h"
#include "bsp/gpio.h"
#include "heartbeat/heartbeat.h"
#include "led/led.h"
#include "rpc/rpc.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#include "transport/transport.h"

static const transport_t uart_transport = {
    .send          = uart_transmit,
    .get_rx_stream = uart_get_rx_stream,
};
#endif /* CORE_CM4 */

void app_main(void)
{
#ifdef CORE_CM4
    uart_init();
    transport_register(&uart_transport);
#endif

    bsp_gpio_init();
    rpc_init();
    led_init();
    heartbeat_init();
}
