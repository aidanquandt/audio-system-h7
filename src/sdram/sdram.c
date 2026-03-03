/**
 * SDRAM power-up sequence and timing.
 * Uses BSP primitives and osDelay; no HAL or timing logic in BSP.
 */
#include "sdram/sdram.h"
#include "bsp/sdram.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CORE_CM4

/* Mode register: burst length 1, sequential, CAS 3, write burst single. */
#define SDRAM_MODE_REG 0x0230
/* Refresh count for 100 MHz SDRAM clock, 4096 rows, 64 ms. */
#define SDRAM_REFRESH 1542

void sdram_init(void)
{
    bsp_sdram_cmd_clock_enable();
    vTaskDelay(pdMS_TO_TICKS(1));
    bsp_sdram_cmd_precharge_all();
    bsp_sdram_cmd_auto_refresh(2);
    bsp_sdram_cmd_load_mode(SDRAM_MODE_REG);
    bsp_sdram_set_refresh_rate(SDRAM_REFRESH);
}

#else

void sdram_init(void)
{
    (void)0;
}

#endif /* CORE_CM4 */
