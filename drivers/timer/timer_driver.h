#pragma once

/**
 * Timer driver — application-facing interface for timer functionality.
 *
 * For now provides only the run-time stats clock used by FreeRTOS
 * (configGENERATE_RUN_TIME_STATS). FreeRTOSConfig.h maps the port macros to
 * configureTimerForRunTimeStats() and getRunTimeCounterValue().
 *
 * Call timer_driver_init() from app startup so the DWT cycle counter is
 * enabled explicitly; FreeRTOS may also call configureTimerForRunTimeStats().
 *
 * This driver will be extended later for other timer use from the application.
 */

void timer_driver_init(void);
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
