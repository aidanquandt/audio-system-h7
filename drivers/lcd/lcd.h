#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LCD_DRIVER_WIDTH  480U
#define LCD_DRIVER_HEIGHT 272U

/**
 * LCD driver: serialises DMA2D (R2M fill + M2M copy) and runs callbacks in task context.
 * Call lcd_driver_init() once before any other API (e.g. from display_init).
 *
 * LVGL: use lcd_driver_copy_rect_sync (or _async) for the flush callback (copy render
 * buffer to framebuffer); lcd_driver_fill_rect_* for solid fill; lcd_driver_width/height
 * and lcd_driver_framebuffer() for display size and direct-fb access if needed.
 */

void lcd_driver_init(void);

/** Framebuffer pointer (RGB565). Valid after SDRAM/LTDC init. NULL on non-CM4. */
volatile uint16_t *lcd_driver_framebuffer(void);

/** Panel: release reset, enable, backlight on. Call after init; may block ~20 ms. */
void lcd_driver_panel_init(void);
void lcd_driver_panel_off(void);

uint16_t lcd_driver_width(void);
uint16_t lcd_driver_height(void);

/* ----- R2M: fill rect with solid colour ----- */

/** Fill full screen; blocks until done. */
void lcd_driver_fill_sync(uint16_t colour);

/** Fill rectangle; blocks until done. */
void lcd_driver_fill_rect_sync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour);

/**
 * Fill rectangle (non-blocking). Callback runs in task context when done.
 * @return true if started, false if busy.
 */
bool lcd_driver_fill_rect_async(uint16_t x,
                                uint16_t y,
                                uint16_t w,
                                uint16_t h,
                                uint16_t colour,
                                void (*callback)(void *),
                                void *user_data);

/* ----- M2M: copy buffer to framebuffer ----- */

/** Copy rectangle from buffer to display; blocks until done. */
void lcd_driver_copy_rect_sync(const uint16_t *src,
                               uint32_t        src_stride,
                               uint16_t        dst_x,
                               uint16_t        dst_y,
                               uint16_t        w,
                               uint16_t        h);

/**
 * Copy rectangle (non-blocking). Callback runs in task context when done.
 * @return true if started, false if busy.
 */
bool lcd_driver_copy_rect_async(const uint16_t *src,
                                uint32_t        src_stride,
                                uint16_t        dst_x,
                                uint16_t        dst_y,
                                uint16_t        w,
                                uint16_t        h,
                                void (*callback)(void *),
                                void *user_data);
