#include "bsp/lcd.h"
#include <stddef.h>

#ifdef CORE_CM4

#include "main.h"

#define FRAMEBUFFER ((volatile uint16_t *)0xD0000000UL)
#define FB_PIXELS   (BSP_LCD_WIDTH * BSP_LCD_HEIGHT)

void bsp_lcd_release_reset(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
}

void bsp_lcd_enable(void)
{
    HAL_GPIO_WritePin(LCD_DISPD7_GPIO_Port, LCD_DISPD7_Pin, GPIO_PIN_SET);
}

void bsp_lcd_backlight_on(void)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
}

void bsp_lcd_backlight_off(void)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);
}

void bsp_lcd_fill(uint16_t colour)
{
    volatile uint16_t *fb = FRAMEBUFFER;
    for (uint32_t i = 0; i < FB_PIXELS; i++)
    {
        fb[i] = colour;
    }
}

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return FRAMEBUFFER;
}

#else

void bsp_lcd_release_reset(void) {}
void bsp_lcd_enable(void) {}
void bsp_lcd_backlight_on(void) {}
void bsp_lcd_backlight_off(void) {}
void bsp_lcd_fill(uint16_t colour)
{
    (void)colour;
}

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return NULL;
}

#endif /* CORE_CM4 */
