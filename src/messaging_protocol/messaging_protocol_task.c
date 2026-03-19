#include "src/messaging_protocol/messaging_protocol.h"
#include "src/messaging_protocol/messaging_protocol_framer.h"
#include "src/messaging_protocol/messaging_protocol_handler.h"
#include "messaging_protocol.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <string.h>

#ifdef CORE_CM4

#define MESSAGING_PROTOCOL_TASK_STACK_WORDS 512
#define MESSAGING_PROTOCOL_TASK_PRIORITY    (tskIDLE_PRIORITY + 3)
#define MESSAGING_PROTOCOL_RECV_TIMEOUT_MS  10U

static messaging_protocol_transport_t *s_transport;
static uint8_t s_tx_buf[MESSAGING_PROTOCOL_MAX_FRAME_SIZE + 2];
static SemaphoreHandle_t s_tx_mutex;

static void messaging_protocol_task(void *pvParameters)
{
    messaging_protocol_framer_t framer;
    uint8_t                    recv_buf[64];
    messaging_protocol_Frame   frame;

    (void)pvParameters;
    messaging_protocol_framer_init(&framer);

    for (;;)
    {
        size_t n = s_transport->recv(s_transport->ctx, recv_buf, sizeof(recv_buf),
                                     MESSAGING_PROTOCOL_RECV_TIMEOUT_MS);

        if (n == 0)
        {
            /* Timeout - check if we're stuck mid-frame (payload timeout) */
            if (framer.state == MESSAGING_PROTOCOL_FRAMER_WAIT_PAYLOAD)
            {
                /* Could add timeout counter here; for now just continue */
            }
            continue;
        }

        for (size_t i = 0; i < n; i++)
        {
            if (messaging_protocol_framer_feed(&framer, recv_buf[i]))
            {
                /* Complete frame */
                pb_istream_t stream = pb_istream_from_buffer(framer.payload, framer.payload_len);
                if (pb_decode(&stream, messaging_protocol_Frame_fields, &frame))
                {
                    messaging_protocol_handler_dispatch(&frame);
                }
                messaging_protocol_framer_reset(&framer);
            }
        }
    }
}

#endif /* CORE_CM4 */

void messaging_protocol_init(messaging_protocol_transport_t *transport)
{
#ifdef CORE_CM4
    if (transport == NULL)
    {
        return;
    }
    s_transport = transport;
    s_tx_mutex  = xSemaphoreCreateMutex();
    configASSERT(s_tx_mutex != NULL);
    xTaskCreate(messaging_protocol_task, "msg_proto", MESSAGING_PROTOCOL_TASK_STACK_WORDS,
                NULL, MESSAGING_PROTOCOL_TASK_PRIORITY, NULL);
#else
    (void)transport;
#endif /* CORE_CM4 */
}

void messaging_protocol_send_frame(messaging_protocol_Frame *frame)
{
#ifdef CORE_CM4
    if (s_transport == NULL || frame == NULL)
    {
        return;
    }

    if (xSemaphoreTake(s_tx_mutex, portMAX_DELAY) != pdTRUE)
    {
        return;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(s_tx_buf + 2, sizeof(s_tx_buf) - 2);
    if (!pb_encode(&stream, messaging_protocol_Frame_fields, frame))
    {
        xSemaphoreGive(s_tx_mutex);
        return;
    }

    size_t payload_len = stream.bytes_written;
    if (payload_len > MESSAGING_PROTOCOL_MAX_FRAME_SIZE)
    {
        xSemaphoreGive(s_tx_mutex);
        return;
    }

    s_tx_buf[0] = (uint8_t)(payload_len & 0xFF);
    s_tx_buf[1] = (uint8_t)((payload_len >> 8) & 0xFF);

    s_transport->send(s_transport->ctx, s_tx_buf, payload_len + 2);
    xSemaphoreGive(s_tx_mutex);
#else
    (void)frame;
#endif /* CORE_CM4 */
}
