#include "src/messaging_protocol/messaging_protocol_framer.h"
#include <string.h>

void messaging_protocol_framer_init(messaging_protocol_framer_t *framer)
{
    framer->state          = MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO;
    framer->payload_len    = 0;
    framer->payload_received = 0;
    memset(framer->payload, 0, sizeof(framer->payload));
}

bool messaging_protocol_framer_feed(messaging_protocol_framer_t *framer, uint8_t byte)
{
    switch (framer->state)
    {
    case MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO:
        framer->payload_len = (uint16_t)byte;
        framer->state       = MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_HI;
        return false;

    case MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_HI:
        framer->payload_len |= (uint16_t)byte << 8;
        if (framer->payload_len > MESSAGING_PROTOCOL_MAX_FRAME_SIZE)
        {
            /* Invalid length, discard and resync */
            messaging_protocol_framer_reset(framer);
            return false;
        }
        framer->payload_received = 0;
        framer->state           = MESSAGING_PROTOCOL_FRAMER_WAIT_PAYLOAD;
        if (framer->payload_len == 0)
        {
            /* Empty frame, discard */
            framer->state = MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO;
            return false;
        }
        return false;

    case MESSAGING_PROTOCOL_FRAMER_WAIT_PAYLOAD:
        framer->payload[framer->payload_received++] = byte;
        if (framer->payload_received >= framer->payload_len)
        {
            framer->state = MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO;
            return true;
        }
        return false;

    default:
        messaging_protocol_framer_reset(framer);
        return false;
    }
}

void messaging_protocol_framer_reset(messaging_protocol_framer_t *framer)
{
    framer->state          = MESSAGING_PROTOCOL_FRAMER_WAIT_LEN_LO;
    framer->payload_len    = 0;
    framer->payload_received = 0;
}
