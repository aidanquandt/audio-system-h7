#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Touch event: last sampled state.
 * Coordinates are in display space; use lcd_driver_width/height or LCD_DRIVER_* for bounds.
 */
typedef struct touch_state {
    uint16_t x;
    uint16_t y;
    bool     pressed;
} touch_state_t;

/**
 * Initialise touch controller (probes GT911).
 * Call after display/I2C are up (e.g. after display_init).
 */
void touch_driver_init(void);

/**
 * Read touch state from device (I2C poll). Call from a task when notified (e.g. EXTI).
 * @param out  Filled with coordinates and pressed flag.
 * @return true if a valid read completed (out updated), false on read error.
 */
bool touch_driver_read(touch_state_t *out);
