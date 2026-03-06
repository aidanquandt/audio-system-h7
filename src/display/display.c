#include "src/display/display.h"

#ifdef CORE_CM4

#include "FreeRTOS.h"
#include "task.h"
#include "bsp/lcd/lcd.h" /* BSP_LCD_WIDTH, BSP_LCD_HEIGHT */
#include "drivers/lcd/lcd.h"

/* RGB565 colour constants */
#define RGB565_RED   0xF800U
#define RGB565_GREEN 0x07E0U
#define RGB565_BLUE  0x001FU
#define RGB565_BLACK 0x0000U

void display_init(void)
{
    lcd_driver_init();
    bsp_lcd_release_reset();
    vTaskDelay(pdMS_TO_TICKS(20));
    lcd_driver_fill_sync(RGB565_BLACK);
    bsp_lcd_enable();
    bsp_lcd_backlight_on();

    display_draw_edge_boxes();
    display_draw_corner_markers();
}

#define CORNER_SIZE 40U

void display_draw_corner_markers(void)
{
    lcd_driver_fill_rect_sync(0, 0, CORNER_SIZE, CORNER_SIZE, RGB565_RED);
    lcd_driver_fill_rect_sync(BSP_LCD_WIDTH - CORNER_SIZE,
                              0,
                              CORNER_SIZE,
                              CORNER_SIZE,
                              RGB565_GREEN);
    lcd_driver_fill_rect_sync(0,
                              BSP_LCD_HEIGHT - CORNER_SIZE,
                              CORNER_SIZE,
                              CORNER_SIZE,
                              RGB565_BLUE);
    lcd_driver_fill_rect_sync(BSP_LCD_WIDTH - CORNER_SIZE,
                              BSP_LCD_HEIGHT - CORNER_SIZE,
                              CORNER_SIZE,
                              CORNER_SIZE,
                              0xFFFFU);
}

/* Draw a 1-pixel-wide unfilled rectangle at (x,y) with size (w,h). */
static void draw_rect_outline(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (w == 0U || h == 0U)
    {
        return;
    }
    lcd_driver_fill_rect_sync(x, y, w, 1U, colour);          /* top */
    lcd_driver_fill_rect_sync(x, y + h - 1U, w, 1U, colour); /* bottom */
    lcd_driver_fill_rect_sync(x, y, 1U, h, colour);          /* left */
    lcd_driver_fill_rect_sync(x + w - 1U, y, 1U, h, colour); /* right */
}

void display_draw_edge_boxes(void)
{
    /* Red outside, black inside: any red past the white frame suggests overscan. */
    lcd_driver_fill_sync(RGB565_RED);

    /* Visible area: panel often doesn't show last column/row, so use 0..(W-1), 0..(H-1). */
    const uint16_t vis_w = BSP_LCD_WIDTH - 1U;
    const uint16_t vis_h = BSP_LCD_HEIGHT - 1U;
    draw_rect_outline(0U, 0U, vis_w, vis_h, 0xFFFFU);

    /* Inner box: one pixel inset on all sides → size (vis_w - 2), (vis_h - 2). */
    const uint16_t inner_w = vis_w - 2U;
    const uint16_t inner_h = vis_h - 2U;
    draw_rect_outline(1U, 1U, inner_w, inner_h, RGB565_GREEN);

    /* Interior of inner box (inside its 1-pixel border): at (2,2), size (inner_w - 2), (inner_h -
     * 2). */
    const uint16_t interior_w = inner_w - 2U;
    const uint16_t interior_h = inner_h - 2U;
    lcd_driver_fill_rect_sync(2U, 2U, interior_w, interior_h, RGB565_BLACK);
}

#else

void display_init(void) {}
void display_draw_corner_markers(void) {}
void display_draw_edge_boxes(void) {}

#endif /* CORE_CM4 */
