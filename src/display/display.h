#pragma once

/**
 * Display. Real implementation on CM4 (depends on SDRAM; call after sdram_init()).
 * No-op stubs on CM7.
 */
void display_init(void);
void display_test(void);
