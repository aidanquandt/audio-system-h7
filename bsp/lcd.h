#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_LCD_WIDTH  480U
#define BSP_LCD_HEIGHT 272U

void     bsp_lcd_release_reset(void);
void     bsp_lcd_enable(void);
void     bsp_lcd_backlight_on(void);
void     bsp_lcd_backlight_off(void);
void     bsp_lcd_fill(uint16_t colour);

#ifdef __cplusplus
}
#endif
