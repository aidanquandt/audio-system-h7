#pragma once

#include <stdbool.h>

/**
 * Enable the SDRAM clock.
 * Caller must delay ≥1 ms before calling bsp_sdram_configure().
 */
void bsp_sdram_enable_clock(void);

/**
 * Complete SDRAM initialisation: precharge-all, auto-refresh, load-mode,
 * set refresh rate. Must be called after the ≥1 ms post-clock-enable delay.
 */
void bsp_sdram_configure(void);

/**
 * Write/readback test over the first 1 KiB of SDRAM.
 * @return true if all reads matched, false otherwise.
 * Returns true unconditionally on non-CM4 cores.
 */
bool bsp_sdram_test(void);