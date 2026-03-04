/**
 * SDRAM power-up sequence and timing.
 * Uses BSP primitives and vTaskDelay; no HAL or timing logic here.
 */
#include "sdram/sdram.h"
#include "bsp/sdram.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CORE_CM4

void sdram_init(void)
{
    bsp_sdram_enable_clock();
    vTaskDelay(pdMS_TO_TICKS(1));
    bsp_sdram_configure();
}

#else

void sdram_init(void) {}

#endif /* CORE_CM4 */
