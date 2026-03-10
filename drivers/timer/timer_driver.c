/**
 * Timer driver — TIM2 counter for FreeRTOS run-time stats.
 *
 * Uses the STM32 TIM2 32-bit up-counter (TIM2->CNT) as the run-time stats clock.
 * TIM2 is configured and started by CM7; both cores can read the counter.
 */

#include "drivers/timer/timer_driver.h"
#include <stdint.h>
#include "stm32h7xx.h"

void timer_driver_init(void)
{
    /* TIM2 is started during CM7 system init; nothing to do here. */
}

void configureTimerForRunTimeStats(void)
{
    /* FreeRTOS hook: timebase is TIM2->CNT, owned by CM7. */
}

unsigned long getRunTimeCounterValue(void)
{
    return (unsigned long)TIM2->CNT;
}
