#pragma once

/**
 * SDRAM driver — runs power-up sequence and timing.
 * Call once after MX_FMC_Init(), from task context (uses osDelay).
 * No-op when not built for CM4.
 */
void sdram_init(void);