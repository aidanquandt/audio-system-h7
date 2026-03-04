#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_LCD_WIDTH  480U
#define BSP_LCD_HEIGHT 272U

/**
 * Release panel from reset (drive LCD_RST high). Call before enable; allow
 * ≥20 ms before enabling display / backlight.
 */
void bsp_lcd_release_reset(void);

/** Enable display output (LCD_DISP high). */
void bsp_lcd_enable(void);

void bsp_lcd_backlight_on(void);
void bsp_lcd_backlight_off(void);

/** Fill entire framebuffer with one RGB565 colour. */
void bsp_lcd_fill(uint16_t colour);

/**
 * Pointer to the LTDC layer 0 framebuffer (RGB565). Valid only after
 * MX_FMC_Init/MX_LTDC_Init and SDRAM init. NULL on non-CM4.
 */
volatile uint16_t *bsp_lcd_framebuffer(void);

#ifdef __cplusplus
}
#endif
