#pragma once

#include <stdint.h>
#include <stddef.h>

/* Maximum encoded output size for n bytes of input (excluding the 0x00 terminator). */
#define COBS_ENCODED_LEN(n)  ((n) + ((n) / 254U) + 1U)

/* Returns number of bytes written to out (no 0x00 terminator). Caller appends it. */
size_t cobs_encode(const uint8_t *in, size_t in_len, uint8_t *out);

/* Returns decoded length, or -1 on malformed input. */
int cobs_decode(const uint8_t *in, size_t in_len, uint8_t *out);
