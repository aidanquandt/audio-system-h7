#include "src/messaging_protocol/messaging_protocol_handler.h"
#include "drivers/gpio/gpio_driver.h"
#include "src/messaging_protocol/messaging_protocol.h"
#include <string.h>

#ifdef CORE_CM4

static void handle_generic_request(messaging_protocol_GenericRequest* req)
{
    messaging_protocol_GenericResponse resp = messaging_protocol_GenericResponse_init_zero;
    resp.has_header                         = true;
    resp.header.request_id                  = req->has_header ? req->header.request_id : 0;
    resp.header.status                      = messaging_protocol_StatusCode_STATUS_OK;

    if (req->which_payload == messaging_protocol_GenericRequest_set_led_tag)
    {
        messaging_protocol_SetLed* set_led = &req->payload.set_led;
        if (set_led->on)
        {
            gpio_driver_set(GPIO_DRIVER_LED_GREEN);
        }
        else
        {
            gpio_driver_reset(GPIO_DRIVER_LED_GREEN);
        }
        resp.which_payload                 = messaging_protocol_GenericResponse_led_status_tag;
        resp.payload.led_status.on         = set_led->on;
        resp.payload.led_status.brightness = set_led->brightness;
    }

    messaging_protocol_Frame frame = messaging_protocol_Frame_init_zero;
    frame.which_msg                = messaging_protocol_Frame_generic_response_tag;
    frame.msg.generic_response     = resp;
    messaging_protocol_send_frame(&frame);
}

#endif /* CORE_CM4 */

void messaging_protocol_handler_dispatch(messaging_protocol_Frame* frame)
{
#ifdef CORE_CM4
    switch (frame->which_msg)
    {
    case messaging_protocol_Frame_heartbeat_tag:
        /* Ignore or log */
        break;

    case messaging_protocol_Frame_log_message_tag:
        /* Ignore for now */
        break;

    case messaging_protocol_Frame_generic_request_tag:
        handle_generic_request(&frame->msg.generic_request);
        break;

    case messaging_protocol_Frame_generic_response_tag:
        /* Unsolicited response, ignore */
        break;

    case messaging_protocol_Frame_audio_stream_open_tag:
    case messaging_protocol_Frame_audio_stream_close_tag:
    case messaging_protocol_Frame_audio_stream_config_tag:
    case messaging_protocol_Frame_audio_chunk_tag:
        /* Phase 3 */
        break;

    default:
        break;
    }
#else
    (void) frame;
#endif /* CORE_CM4 */
}
