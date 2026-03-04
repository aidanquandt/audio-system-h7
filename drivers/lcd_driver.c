#include "drivers/lcd_driver.h"

#ifdef CORE_CM4

#include "bsp/lcd.h"
#include "FreeRTOS.h"
#include "semphr.h"

static SemaphoreHandle_t fill_done_sem = NULL;
static void (*user_callback)(void *) = NULL;
static void *user_callback_data = NULL;

static void driver_fill_done_cb(void *user_data)
{
    (void)user_data;
    BaseType_t wake = pdFALSE;
    if (fill_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(fill_done_sem, &wake);
        portYIELD_FROM_ISR(wake);
    }
    if (user_callback != NULL)
    {
        void (*cb)(void *) = user_callback;
        void *ud = user_callback_data;
        user_callback = NULL;
        user_callback_data = NULL;
        cb(ud);
    }
}

void lcd_driver_init(void)
{
    if (fill_done_sem == NULL)
    {
        fill_done_sem = xSemaphoreCreateBinary();
        configASSERT(fill_done_sem != NULL);
        xSemaphoreGive(fill_done_sem);
    }
}

bool lcd_driver_fill_async(uint16_t colour, void (*callback)(void *), void *user_data)
{
    if (fill_done_sem == NULL)
    {
        return false;
    }
    if (xSemaphoreTake(fill_done_sem, 0) != pdTRUE)
    {
        return false;
    }
    user_callback = callback;
    user_callback_data = user_data;
    if (!bsp_lcd_fill_async(colour, driver_fill_done_cb, NULL))
    {
        user_callback = NULL;
        user_callback_data = NULL;
        xSemaphoreGive(fill_done_sem);
        return false;
    }
    return true;
}

void lcd_driver_fill_sync(uint16_t colour)
{
    if (fill_done_sem == NULL)
    {
        return;
    }
    if (xSemaphoreTake(fill_done_sem, portMAX_DELAY) != pdTRUE)
    {
        return;
    }
    user_callback = NULL;
    user_callback_data = NULL;
    if (!bsp_lcd_fill_async(colour, driver_fill_done_cb, NULL))
    {
        xSemaphoreGive(fill_done_sem);
        return;
    }
    xSemaphoreTake(fill_done_sem, portMAX_DELAY);  /* block until DMA done (callback gives semaphore) */
    xSemaphoreGive(fill_done_sem);                 /* release so next fill_sync or fill_async can proceed */
}

#else

void lcd_driver_init(void) {}
bool lcd_driver_fill_async(uint16_t colour, void (*callback)(void *), void *user_data)
{
    (void)colour;
    (void)callback;
    (void)user_data;
    return false;
}
void lcd_driver_fill_sync(uint16_t colour) { (void)colour; }

#endif /* CORE_CM4 */
