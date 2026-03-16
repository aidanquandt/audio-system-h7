/**
 * dlog — diagnostic log: FreeRTOS task CPU utilization over RPC/UART.
 *
 * Runs a periodic task on each core; samples uxTaskGetSystemState(), computes
 * utilization percentage per task. External reporting over RPC has been removed.
 *
 * DLOG_MAX_TASKS: max tasks reported per core. If the system has more tasks than
 * this, uxTaskGetSystemState() returns 0 and we send no utilization for that period.
 * Must be at least the expected task count (default 16 to match typical builds).
 *
 * TASK_NAME_LEN must match configMAX_TASK_NAME_LEN in FreeRTOSConfig.h (wire format).
 */

#include "src/dlog/dlog.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#ifndef DLOG_MAX_TASKS
#define DLOG_MAX_TASKS 16
#endif

#ifndef DLOG_PERIOD_MS
#define DLOG_PERIOD_MS 2000
#endif

/* Must match configMAX_TASK_NAME_LEN; task_name in proto is 16 bytes. */
#define TASK_NAME_LEN 16

static void dlog_task(void *pvParameters)
{
    (void)pvParameters;

    static TaskStatus_t status[DLOG_MAX_TASKS];
    static TaskStatus_t prev_status[DLOG_MAX_TASKS];
    static UBaseType_t  prev_n         = 0;
    static uint32_t     prev_total_now = 0;
    static uint8_t      first_run      = 1;

    for (;;)
    {
        uint32_t    total_now = 0;
        UBaseType_t n         = uxTaskGetSystemState(status, DLOG_MAX_TASKS, &total_now);

        if (n == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(DLOG_PERIOD_MS));
            continue;
        }

        if (first_run)
        {
            memcpy(prev_status, status, (size_t)n * sizeof(TaskStatus_t));
            prev_n         = n;
            prev_total_now = total_now;
            first_run      = 0;
            vTaskDelay(pdMS_TO_TICKS(DLOG_PERIOD_MS));
            continue;
        }

        /*
         * First pass: compute run_delta per task (match by handle).
         * Percentage denominator uses kernel total_delta (see below), not sum of deltas.
         */
        uint32_t run_deltas[DLOG_MAX_TASKS];
        for (UBaseType_t i = 0; i < n; i++)
        {
            uint32_t prev_run = 0;
            for (UBaseType_t j = 0; j < prev_n; j++)
            {
                if (prev_status[j].xHandle == status[i].xHandle)
                {
                    prev_run = prev_status[j].ulRunTimeCounter;
                    break;
                }
            }
            if (status[i].ulRunTimeCounter >= prev_run)
            {
                run_deltas[i] = status[i].ulRunTimeCounter - prev_run;
            }
            else
            {
                run_deltas[i] = 0; /* counter wrap or new task */
            }
        }

        /*
         * Use kernel total run-time delta as denominator so percentages are
         * normalized to actual elapsed time. If we used sum_delta and it
         * undercounts (e.g. task ordering, new/deleted tasks), percentages
         * get inflated. total_delta is from the same counter as ulRunTimeCounter.
         */
        uint32_t total_delta = (uint32_t)(total_now - prev_total_now);
        uint64_t denominator = (total_delta > 0U) ? (uint64_t)total_delta : 1ULL;

        /*
         * Second pass: compute percentage per task (truncated). Assign rounding
         * remainder to the task with largest run_delta so reported total is 100%.
         */
        uint8_t pcts[DLOG_MAX_TASKS];
        uint32_t sum_pct = 0;
        UBaseType_t idx_max = 0;
        for (UBaseType_t i = 0; i < n; i++)
        {
            uint32_t pct = (uint32_t)((100ULL * (uint64_t)run_deltas[i]) / denominator);
            if (pct > 100U)
            {
                pct = 100U;
            }
            pcts[i] = (uint8_t)pct;
            sum_pct += pcts[i];
            if (run_deltas[i] > run_deltas[idx_max])
            {
                idx_max = i;
            }
        }
        if (sum_pct < 100U && n > 0)
        {
            uint32_t rem = 100U - sum_pct;
            uint32_t new_pct = (uint32_t)pcts[idx_max] + rem;
            pcts[idx_max] = (uint8_t)(new_pct > 100U ? 100U : new_pct);
        }

        for (UBaseType_t i = 0; i < n; i++)
        {
            (void)pcts[i];
        }

        memcpy(prev_status, status, (size_t)n * sizeof(TaskStatus_t));
        prev_n         = n;
        prev_total_now = total_now;

        vTaskDelay(pdMS_TO_TICKS(DLOG_PERIOD_MS));
    }
}

void dlog_init(void)
{
    xTaskCreate(dlog_task, "dlog", 384, NULL, tskIDLE_PRIORITY + 1, NULL);
}
