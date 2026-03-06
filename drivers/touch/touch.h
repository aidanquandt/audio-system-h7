#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Display dimensions for scaling touch coordinates (480x272). */
#define TOUCH_DISPLAY_W 480U
#define TOUCH_DISPLAY_H 272U

/**
 * Touch event: last sampled state.
 * Coordinates are in display space [0, TOUCH_DISPLAY_W) x [0, TOUCH_DISPLAY_H).
 */
typedef struct touch_state
{
    uint16_t x;
    uint16_t y;
    bool     pressed;
} touch_state_t;

/**
 * Initialise touch controller (probes GT911 then FT5336).
 * Call after display/I2C are up (e.g. after display_init).
 * @return true if a known touch IC was detected, false otherwise.
 */
bool touch_init(void);

/**
 * Read last touch state (polling). Call periodically from a task.
 * @param out  Filled with last coordinates and pressed flag.
 * @return true if out was updated, false on read error or no touch.
 */
bool touch_get_last(touch_state_t *out);

#ifdef __cplusplus
}
#endif
