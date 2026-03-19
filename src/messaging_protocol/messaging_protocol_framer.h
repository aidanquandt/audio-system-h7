#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MESSAGING_PROTOCOL_MAX_FRAME_SIZE 1024U
#define MESSAGING_PROTOCOL_FRAME_PAYLOAD_TIMEOUT_MS 100U

typedef enum {
    MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO,
    MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_HI,
    MESSAGING_PROTOCOL_FRAMER_WAIT_PAYLOAD,
} messaging_protocol_framer_state_t;

typedef struct messaging_protocol_framer {
    messaging_protocol_framer_state_t state;
    uint16_t                          payload_len;
    uint16_t                          payload_received;
    uint8_t                           payload[MESSAGING_PROTOCOL_MAX_FRAME_SIZE];
} messaging_protocol_framer_t;

void messaging_protocol_framer_init(messaging_protocol_framer_t *framer);

/* Feed one byte. Returns true when a complete frame is ready in framer->payload,
 * framer->payload_len set. Caller must copy/decode and then call reset. */
bool messaging_protocol_framer_feed(messaging_protocol_framer_t *framer, uint8_t byte);

/* Reset state after consuming a frame (or on timeout/error). */
void messaging_protocol_framer_reset(messaging_protocol_framer_t *framer);
