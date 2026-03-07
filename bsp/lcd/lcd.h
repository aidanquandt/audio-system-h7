#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BSP_LCD_WIDTH  480U
#define BSP_LCD_HEIGHT 272U

/** Panel control (call before any DMA2D use). */
void bsp_lcd_release_reset(void);
void bsp_lcd_enable(void);
void bsp_lcd_backlight_on(void);
void bsp_lcd_backlight_off(void);

/**
 * Framebuffer pointer (RGB565). Valid after SDRAM/LTDC init. NULL on non-CM4.
 */
volatile uint16_t *bsp_lcd_framebuffer(void);

/* ----- R2M: fill rect with solid colour ----- */

/**
 * Fill a rectangle with one RGB565 colour (non-blocking).
 * Full-screen: pass (0, 0, BSP_LCD_WIDTH, BSP_LCD_HEIGHT).
 * Callback may run in interrupt context; keep it short.
 *
 * @return true if transfer was started, false if DMA2D busy or callback NULL.
 */
bool bsp_lcd_fill_rect_async(uint16_t x,
                             uint16_t y,
                             uint16_t w,
                             uint16_t h,
                             uint16_t colour,
                             void (*callback)(void *),
                             void *user_data);

/* ----- M2M: copy buffer to framebuffer ----- */

/**
 * Copy a rectangle from a source buffer to the framebuffer (non-blocking).
 * Source is RGB565; src_stride is line pitch in pixels (use w for tight buffer).
 * Callback may run in interrupt context; keep it short.
 *
 * @return true if transfer was started, false if DMA2D busy or callback NULL.
 */
bool bsp_lcd_copy_rect_async(const uint16_t *src,
                             uint32_t        src_stride,
                             uint16_t        dst_x,
                             uint16_t        dst_y,
                             uint16_t        w,
                             uint16_t        h,
                             void (*callback)(void *),
                             void *user_data);
