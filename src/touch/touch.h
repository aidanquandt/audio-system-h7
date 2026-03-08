#pragma once

/**
 * Application-layer touch: start the task that handles touch input (e.g. draws
 * a dot at each touch). Call after display_init() and touch_driver_init().
 * No-op on CM7.
 */
void touch_init(void);
