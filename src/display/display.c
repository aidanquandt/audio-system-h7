#include "src/display/display.h"

#ifdef CORE_CM4

#include "FreeRTOS.h"
#include "task.h"
#include "bsp/lcd/lcd.h"
#include "drivers/lcd/lcd.h"

/* RGB565 colour constants */
#define RGB565_RED   0xF800U
#define RGB565_GREEN 0x07E0U
#define RGB565_BLUE  0x001FU
#define RGB565_BLACK 0x0000U

void display_init(void)
{
    lcd_init();
    bsp_lcd_release_reset();
    vTaskDelay(pdMS_TO_TICKS(20));
    lcd_fill_sync(RGB565_BLACK);
    bsp_lcd_enable();
    bsp_lcd_backlight_on();
}

void display_test(void)
{
    static const uint16_t colours[] = {RGB565_RED, RGB565_GREEN, RGB565_BLUE};

    for (uint32_t i = 0; i < 3; i++)
    {
        lcd_fill_sync(colours[i]);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    lcd_fill_sync(RGB565_BLACK);
}

#else

void display_init(void) {}
void display_test(void) {}

#endif /* CORE_CM4 */
