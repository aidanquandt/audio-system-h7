#pragma once

/**
 * Display (CM4 only). Depends on SDRAM being initialized first; call after
 * sdram_init() from the same task.
 */
void display_init(void);
void display_test(void);
