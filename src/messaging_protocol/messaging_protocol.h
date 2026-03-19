#pragma once

#include "src/messaging_protocol/messaging_protocol_transport.h"
#include "messaging_protocol.pb.h"

/* Call from app_main after uart_driver_init(). Creates and starts the protocol task. */
void messaging_protocol_init(messaging_protocol_transport_t *transport);

/* Encode frame, add length prefix, send via transport. Thread-safe (can be called from any task). */
void messaging_protocol_send_frame(messaging_protocol_Frame *frame);

/* Initialize transport struct for UART. Call before messaging_protocol_init. */
void messaging_protocol_uart_transport_init(messaging_protocol_transport_t *transport);
