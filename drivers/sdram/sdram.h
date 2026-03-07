#pragma once

#include <stdbool.h>

/**
 * SDRAM driver: power-up sequence and timing.
 * Call sdram_driver_init() once after MX_FMC_Init(), from task context (uses vTaskDelay).
 */
void sdram_driver_init(void);

void sdram_driver_enable_clock(void);
void sdram_driver_configure(void);
bool sdram_driver_test(void);
