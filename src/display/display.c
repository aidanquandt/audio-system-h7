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

#define CORNER_SIZE 40U
#define DISP_W      480U
#define DISP_H      272U

void display_draw_corner_markers(void)
{
    lcd_fill_sync(RGB565_BLACK);
    /* Top-left (0,0) = red */
    lcd_fill_rect_sync(0, 0, CORNER_SIZE, CORNER_SIZE, RGB565_RED);
    /* Top-right (DISP_W - CORNER_SIZE, 0) = green */
    lcd_fill_rect_sync(DISP_W - CORNER_SIZE, 0, CORNER_SIZE, CORNER_SIZE, RGB565_GREEN);
    /* Bottom-left (0, DISP_H - CORNER_SIZE) = blue */
    lcd_fill_rect_sync(0, DISP_H - CORNER_SIZE, CORNER_SIZE, CORNER_SIZE, RGB565_BLUE);
    /* Bottom-right = white */
    lcd_fill_rect_sync(DISP_W - CORNER_SIZE, DISP_H - CORNER_SIZE, CORNER_SIZE, CORNER_SIZE, 0xFFFFU);
}

/* Draw a 1-pixel-wide unfilled rectangle at (x,y) with size (w,h). */
static void draw_rect_outline(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    if (w == 0U || h == 0U) return;
    lcd_fill_rect_sync(x, y, w, 1U, colour);                    /* top */
    lcd_fill_rect_sync(x, y + h - 1U, w, 1U, colour);           /* bottom */
    lcd_fill_rect_sync(x, y, 1U, h, colour);                    /* left */
    lcd_fill_rect_sync(x + w - 1U, y, 1U, h, colour);           /* right */
}

void display_draw_edge_boxes(void)
{
    /* Red outside, black inside: any red past the white frame suggests overscan. */
    lcd_fill_sync(RGB565_RED);
    /* Panel visible area is typically 0..478 x 0..270 (last column/row not shown).
     * Outer box: last visible pixel at right/bottom (478, 270). */
    draw_rect_outline(0U, 0U, DISP_W - 1U, DISP_H - 1U, 0xFFFFU);   /* white */
    /* Inner box: one pixel smaller on all sides */
    draw_rect_outline(1U, 1U, DISP_W - 3U, DISP_H - 3U, RGB565_GREEN);
    /* Black fill inside the green box (from (2,2) to (476,268)) */
    lcd_fill_rect_sync(2U, 2U, DISP_W - 5U, DISP_H - 5U, RGB565_BLACK);
}

#else

void display_init(void) {}
void display_test(void) {}
void display_draw_corner_markers(void) {}
void display_draw_edge_boxes(void) {}

#endif /* CORE_CM4 */
