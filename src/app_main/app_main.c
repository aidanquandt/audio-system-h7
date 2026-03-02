#include "app_main/app_main.h"
#include "ipc/ipc.h"
#include "heartbeat/heartbeat.h"
#include "rpc/rpc.h"
#include "rpc/messages.h"
#include "bsp/gpio.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#include "transport/transport.h"

static const transport_t uart_transport = {
    .send          = uart_transmit,
    .get_rx_stream = uart_get_rx_stream,
};

static void on_led_toggle_green(uint8_t msg_id, const uint8_t *payload, size_t len)
{
    (void)msg_id; (void)payload; (void)len;
    bsp_gpio_toggle(BSP_GPIO_LED_GREEN);
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
static void on_led_toggle_red(uint8_t msg_id, const uint8_t *payload, size_t len)
{
    (void)msg_id; (void)payload; (void)len;
    bsp_gpio_toggle(BSP_GPIO_LED_RED);
}
#endif /* CORE_CM7 */

void app_main(void)
{
    ipc_init();

#ifdef CORE_CM4
    uart_init();
    transport_register(&uart_transport);
    rpc_register(MSG_LED_TOGGLE_GREEN, DEST_CM4, on_led_toggle_green);
    rpc_register(MSG_LED_TOGGLE_RED,   DEST_CM7, NULL);
#endif

#ifdef CORE_CM7
    rpc_register(MSG_LED_TOGGLE_RED, DEST_CM7, on_led_toggle_red);
#endif

    rpc_init();
    heartbeat_init();
}

