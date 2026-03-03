/**
 * SDRAM power-up sequence and timing.
 * Uses BSP primitives and vTaskDelay; no HAL or timing logic here.
 */
#include "sdram/sdram.h"
#include "bsp/sdram.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CORE_CM4

bool sdram_init(void)
{
    bsp_sdram_enable_clock();
    vTaskDelay(pdMS_TO_TICKS(1));
    bsp_sdram_configure();
    return bsp_sdram_test();
}

#else

bool sdram_init(void) { return true; }

#endif /* CORE_CM4 */
