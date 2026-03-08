#include "src/transport/transport.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CORE_CM4
#include "drivers/uart/uart_driver.h"
#endif

#define MAX_TRANSPORTS 4U

static const transport_t *transports[MAX_TRANSPORTS];
static size_t             count = 0;

#ifdef CORE_CM4
static const transport_t uart_transport = {
    .send          = uart_driver_transmit,
    .get_rx_stream = uart_driver_get_rx_stream,
};
#endif

void transport_init(void)
{
#ifdef CORE_CM4
    count = 0;
    transport_register(&uart_transport);
#endif
}

void transport_register(const transport_t *t)
{
    configASSERT(count < MAX_TRANSPORTS);
    transports[count++] = t;
}

void transport_send(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < count; i++)
    {
        if (transports[i]->send)
        {
            transports[i]->send(buf, len);
        }
    }
}

size_t transport_count(void)
{
    return count;
}

const transport_t *transport_get(size_t idx)
{
    configASSERT(idx < count);
    return transports[idx];
}
