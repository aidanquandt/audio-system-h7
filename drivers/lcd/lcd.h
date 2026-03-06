#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the LCD driver (binary semaphore for DMA2D fill serialisation).
 * Call once before any lcd_fill_async or lcd_fill_sync (e.g. from display_init).
 */
void lcd_init(void);

/**
 * Start a DMA2D fill of the entire LCD framebuffer (non-blocking).
 * Takes the driver semaphore to serialise with other fills, starts the transfer
 * via BSP, and returns. When the transfer completes, the driver gives the
 * semaphore and calls callback with user_data. Callback may run in ISR context.
 *
 * @return true if the fill was started, false if the driver was busy (semaphore held).
 */
bool lcd_fill_async(uint16_t colour, void (*callback)(void *), void *user_data);

/**
 * Fill the framebuffer and block until the DMA2D transfer completes.
 * Convenience wrapper over lcd_fill_async; safe to call from any task.
 */
void lcd_fill_sync(uint16_t colour);

/**
 * Fill a rectangle and block until done. Safe to call from any task.
 */
void lcd_fill_rect_sync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour);

#ifdef __cplusplus
}
#endif
