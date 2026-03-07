#pragma once

#include <stdbool.h>

/**
 * Application-layer touch: start the task that handles touch input (e.g. draws
 * a dot at each touch). Call after display_init() and touch_driver_init().
 * No-op on CM7.
 * @return true if the touch task and EXTI were set up, false on failure.
 */
bool touch_init(void);
