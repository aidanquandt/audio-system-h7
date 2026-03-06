#pragma once

#include <stdbool.h>
#include <stdint.h>

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

/**
 * Start a DMA2D fill of the entire framebuffer with one RGB565 colour (non-blocking).
 * When the transfer completes or errors, callback is invoked with user_data.
 * Callback may run in interrupt context; keep it short (e.g. give a semaphore).
 *
 * @return true if the transfer was started, false if DMA2D was busy or callback was NULL.
 */
bool bsp_lcd_fill_async(uint16_t colour, void (*callback)(void *), void *user_data);

/**
 * Pointer to the LTDC layer 0 framebuffer (RGB565). Valid only after
 * MX_FMC_Init/MX_LTDC_Init and SDRAM init. NULL on non-CM4.
 */
volatile uint16_t *bsp_lcd_framebuffer(void);

/**
 * Fill a rectangle with one RGB565 colour (non-blocking). Same callback
 * semantics as bsp_lcd_fill_async. Clamped to framebuffer bounds.
 */
bool bsp_lcd_fill_rect_async(uint16_t x,
                             uint16_t y,
                             uint16_t w,
                             uint16_t h,
                             uint16_t colour,
                             void (*callback)(void *),
                             void *user_data);
