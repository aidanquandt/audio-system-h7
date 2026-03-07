#include "drivers/lcd/lcd.h"

#ifdef CORE_CM4

#include "bsp/lcd/lcd.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static SemaphoreHandle_t fill_done_sem  = NULL;
static SemaphoreHandle_t sync_done_sem  = NULL;
static void (*user_callback)(void *)    = NULL;
static void *user_callback_data         = NULL;
static volatile bool sync_pending       = false;

/* ISR: only signal completion; user callback runs in worker task context. */
static void lcd_driver_fill_done_cb(void *user_data)
{
    (void)user_data;
    BaseType_t wake = pdFALSE;
    if (fill_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(fill_done_sem, &wake);
        portYIELD_FROM_ISR(wake);
    }
}

static void lcd_driver_worker_task(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        if (xSemaphoreTake(fill_done_sem, portMAX_DELAY) != pdTRUE)
        {
            continue;
        }
        if (user_callback != NULL)
        {
            void (*cb)(void *) = user_callback;
            void *ud            = user_callback_data;
            user_callback       = NULL;
            user_callback_data  = NULL;
            cb(ud);
        }
        else if (sync_pending)
        {
            xSemaphoreGive(sync_done_sem);
        }
        xSemaphoreGive(fill_done_sem);
    }
}

void lcd_driver_init(void)
{
    if (fill_done_sem == NULL)
    {
        fill_done_sem = xSemaphoreCreateBinary();
        configASSERT(fill_done_sem != NULL);
        sync_done_sem = xSemaphoreCreateBinary();
        configASSERT(sync_done_sem != NULL);
        xSemaphoreGive(fill_done_sem);
        xTaskCreate(lcd_driver_worker_task, "lcd_worker", 256, NULL, 1, NULL);
    }
}

void lcd_driver_panel_init(void)
{
    bsp_lcd_release_reset();
    vTaskDelay(pdMS_TO_TICKS(20));
    bsp_lcd_enable();
    bsp_lcd_backlight_on();
}

void lcd_driver_panel_off(void)
{
    bsp_lcd_backlight_off();
}

uint16_t lcd_driver_width(void)
{
    return (uint16_t)BSP_LCD_WIDTH;
}

uint16_t lcd_driver_height(void)
{
    return (uint16_t)BSP_LCD_HEIGHT;
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
    if (!bsp_lcd_fill_async(colour, lcd_driver_fill_done_cb, NULL))
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
    if (fill_done_sem == NULL || sync_done_sem == NULL)
    {
        return;
    }
    if (xSemaphoreTake(fill_done_sem, portMAX_DELAY) != pdTRUE)
    {
        return;
    }
    user_callback      = NULL;
    user_callback_data = NULL;
    sync_pending       = true;
    if (!bsp_lcd_fill_async(colour, lcd_driver_fill_done_cb, NULL))
    {
        sync_pending = false;
        xSemaphoreGive(fill_done_sem);
        return;
    }
    xSemaphoreTake(sync_done_sem, portMAX_DELAY);
    sync_pending = false;
}

void lcd_driver_fill_rect_sync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (fill_done_sem == NULL || sync_done_sem == NULL)
    {
        return;
    }
    if (xSemaphoreTake(fill_done_sem, portMAX_DELAY) != pdTRUE)
    {
        return;
    }
    user_callback      = NULL;
    user_callback_data = NULL;
    sync_pending       = true;
    if (!bsp_lcd_fill_rect_async(x, y, w, h, colour, lcd_driver_fill_done_cb, NULL))
    {
        sync_pending = false;
        xSemaphoreGive(fill_done_sem);
        return;
    }
    xSemaphoreTake(sync_done_sem, portMAX_DELAY);
    sync_pending = false;
}

#else

void lcd_driver_init(void) {}
void lcd_driver_panel_init(void) {}
void lcd_driver_panel_off(void) {}
uint16_t lcd_driver_width(void) { return 0U; }
uint16_t lcd_driver_height(void) { return 0U; }
bool lcd_driver_fill_async(uint16_t colour, void (*callback)(void *), void *user_data)
{
    (void)colour;
    (void)callback;
    (void)user_data;
    return false;
}
void lcd_driver_fill_sync(uint16_t colour) { (void)colour; }
void lcd_driver_fill_rect_sync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)colour;
}

#endif /* CORE_CM4 */
