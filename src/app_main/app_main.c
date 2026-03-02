#include "app_main/app_main.h"
#include "ipc/ipc.h"
#include "heartbeat/heartbeat.h"
#include "rpc/rpc.h"
#include "rpc/messages.h"

#ifdef CORE_CM4
#include "uart/uart.h"
#include "transport/transport.h"
#include "FreeRTOS.h"
#include "task.h"

static const transport_t uart_transport = {
    .send          = uart_transmit,
    .get_rx_stream = uart_get_rx_stream,
};

static void on_ping(uint8_t msg_id, const uint8_t *payload, size_t len)
{
    (void)msg_id;
    if (len < sizeof(ping_t)) return;
    const ping_t *ping = (const ping_t *)payload;
    pong_t pong = { .seq = ping->seq };
    rpc_transmit(MSG_PONG, &pong, sizeof(pong));
}
#endif /* CORE_CM4 */

void app_main(void)
{
    ipc_init();

#ifdef CORE_CM4
    uart_init();
    transport_register(&uart_transport);
    rpc_register(MSG_PING,     DEST_CM4, on_ping);
    rpc_register(MSG_SET_GAIN, DEST_CM7, NULL);   /* forwarded to CM7 */
#endif

    rpc_init();
    heartbeat_init();
}

