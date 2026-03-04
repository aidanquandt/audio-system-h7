#pragma once

/**
 * Run the SDRAM power-up sequence.
 * Call once after MX_FMC_Init(), from task context (uses vTaskDelay).
 */
void sdram_init(void);
