/**
 * Timer driver — DWT cycle counter for FreeRTOS run-time stats.
 *
 * Uses the Cortex-M DWT (Data Watchpoint and Trace) CYCCNT register.
 * Each core (CM4/CM7) has its own DWT, so the same code runs on both;
 * each reads its own cycle counter.
 *
 * Register base addresses match ARM CMSIS: CoreDebug DEMCR 0xE000EDFC,
 * DWT_Type CTRL 0xE0001000, CYCCNT 0xE0001004. When building with CMSIS
 * in the include path, DWT and CoreDebug from core_cm4.h/core_cm7.h
 * could be used instead of the macros below.
 */

#include "drivers/timer/timer_driver.h"
#include <stdint.h>

/* DWT and CoreDebug (addresses per ARM CMSIS DWT_Type / CoreDebug_Type) */
#define DWT_CTRL   (*(volatile uint32_t *)0xE0001000UL)
#define DWT_CYCCNT (*(volatile uint32_t *)0xE0001004UL)
#define DEMCR      (*(volatile uint32_t *)0xE000EDFCUL)

#define DEMCR_TRCENA        (1U << 24)
#define DWT_CTRL_CYCCNTENA  (1U << 0)
#define DWT_CTRL_NOCYCCNT   (1U << 25)

static void enable_dwt_cycle_counter(void)
{
    DEMCR   |= DEMCR_TRCENA;
    DWT_CYCCNT = 0;
    DWT_CTRL  &= ~DWT_CTRL_NOCYCCNT;
    DWT_CTRL  |= DWT_CTRL_CYCCNTENA;
}

void timer_driver_init(void)
{
    enable_dwt_cycle_counter();
}

void configureTimerForRunTimeStats(void)
{
    enable_dwt_cycle_counter();
}

unsigned long getRunTimeCounterValue(void)
{
    return (unsigned long)DWT_CYCCNT;
}
