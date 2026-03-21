#include "FreeRTOS.h"
#include "drivers/uart/uart_driver.h"
#include "src/messaging_protocol/messaging_protocol_transport.h"
#include "stream_buffer.h"
#include <stddef.h>

#ifdef CORE_CM4

static int uart_send(void* ctx, const uint8_t* buf, size_t len)
{
    (void) ctx;
    if (buf == NULL || len == 0)
    {
        return -1;
    }
    uart_driver_transmit(buf, len);
    return 0;
}

static size_t uart_recv(void* ctx, uint8_t* buf, size_t max_len, uint32_t timeout_ms)
{
    (void) ctx;
    if (buf == NULL || max_len == 0)
    {
        return 0;
    }
    StreamBufferHandle_t stream = uart_driver_get_rx_stream();
    if (stream == NULL)
    {
        return 0;
    }
    size_t received = xStreamBufferReceive(stream, buf, max_len, pdMS_TO_TICKS(timeout_ms));
    return received;
}

void messaging_protocol_uart_transport_init(messaging_protocol_transport_t* transport)
{
    transport->send = uart_send;
    transport->recv = uart_recv;
    transport->ctx  = NULL;
}

#else

void messaging_protocol_uart_transport_init(messaging_protocol_transport_t* transport)
{
    (void) transport;
}

#endif /* CORE_CM4 */
