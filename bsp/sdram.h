#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Send CLK_ENABLE to SDRAM bank 2. */
void bsp_sdram_cmd_clock_enable(void);

/** Send precharge-all to SDRAM bank 2. */
void bsp_sdram_cmd_precharge_all(void);

/** Send auto-refresh command; @a count cycles (typically 2). */
void bsp_sdram_cmd_auto_refresh(uint8_t count);

/** Send load-mode command; @a mode_reg e.g. 0x0230 (burst 1, CAS 3, etc.). */
void bsp_sdram_cmd_load_mode(uint32_t mode_reg);

/** Set SDRAM refresh rate register (e.g. 1542 for 100 MHz / 4096 rows / 64 ms). */
void bsp_sdram_set_refresh_rate(uint32_t count);

/**
 * Write/readback test over the first 1 KiB of SDRAM.
 * @return true if all reads matched, false otherwise.
 * No-op on cores without SDRAM (returns true).
 */
bool bsp_sdram_test(void);

#ifdef __cplusplus
}
#endif
