#pragma once

#include "messaging_protocol.pb.h"

/* Dispatch a decoded frame to domain handlers. Called from the protocol task.
 * Handlers may call messaging_protocol_send_frame() to send responses. */
void messaging_protocol_handler_dispatch(messaging_protocol_Frame *frame);
