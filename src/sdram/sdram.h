#pragma once

#include <stdbool.h>

/**
 * Run the SDRAM power-up sequence and verify with a read/write test.
 * Call once after MX_FMC_Init(), from task context (uses vTaskDelay).
 * @return true if the post-init test passes, false otherwise.
 * Returns true unconditionally on non-CM4 cores.
 */
bool sdram_init(void);