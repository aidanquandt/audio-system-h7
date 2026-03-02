#include "transport/transport.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_TRANSPORTS 4U

static const transport_t *s_transports[MAX_TRANSPORTS];
static size_t             s_count = 0;

void transport_register(const transport_t *t)
{
    configASSERT(s_count < MAX_TRANSPORTS);
    s_transports[s_count++] = t;
}

void transport_send(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < s_count; i++) {
        if (s_transports[i]->send) {
            s_transports[i]->send(buf, len);
        }
    }
}

size_t transport_count(void) { return s_count; }

const transport_t *transport_get(size_t idx)
{
    configASSERT(idx < s_count);
    return s_transports[idx];
}
