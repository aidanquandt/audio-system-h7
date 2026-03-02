#include "cobs/cobs.h"

size_t cobs_encode(const uint8_t *in, size_t in_len, uint8_t *out)
{
    size_t write    = 0;
    size_t code_pos = write++;
    uint8_t code    = 1;

    for (size_t i = 0; i < in_len; i++) {
        if (in[i] == 0x00) {
            out[code_pos] = code;
            code_pos = write++;
            code = 1;
        } else {
            out[write++] = in[i];
            code++;
            if (code == 0xFF) {
                out[code_pos] = code;
                code_pos = write++;
                code = 1;
            }
        }
    }

    out[code_pos] = code;
    return write;
}

int cobs_decode(const uint8_t *in, size_t in_len, uint8_t *out)
{
    size_t read = 0, write = 0;

    while (read < in_len) {
        uint8_t code = in[read++];
        if (code == 0x00) return -1;

        for (uint8_t i = 1; i < code; i++) {
            if (read >= in_len) return -1;
            out[write++] = in[read++];
        }

        /* All blocks except the last emit a trailing zero. */
        if (code < 0xFF && read < in_len) {
            out[write++] = 0x00;
        }
    }

    return (int)write;
}
