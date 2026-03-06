#include "bsp/lcd/lcd.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef CORE_CM4

#include "dma2d.h"
#include "main.h"

#define FRAMEBUFFER ((volatile uint16_t *)0xD0000000UL)

static void (*fill_callback)(void *) = NULL;
static void *fill_user_data          = NULL;
static bool  rect_restore_offset      = false;

static void dma2d_fill_cplt_shim(DMA2D_HandleTypeDef *hdma2d)
{
    (void)hdma2d;
    if (rect_restore_offset)
    {
        hdma2d->Init.OutputOffset = 0;
        rect_restore_offset       = false;
    }
    if (fill_callback != NULL)
    {
        void (*cb)(void *) = fill_callback;
        void *ud           = fill_user_data;
        fill_callback      = NULL;
        fill_user_data     = NULL;
        cb(ud);
    }
}

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

/* HAL_DMA2D_Start_IT R2M mode expects colour in 32-bit ARGB8888; framebuffer is RGB565. */
static uint32_t rgb565_to_argb8888(uint16_t c)
{
    uint32_t r5 = (c >> 11U) & 0x1FU;
    uint32_t g6 = (c >> 5U) & 0x3FU;
    uint32_t b5 = c & 0x1FU;
    uint32_t r8 = (r5 << 3U) | (r5 >> 2U);
    uint32_t g8 = (g6 << 2U) | (g6 >> 4U);
    uint32_t b8 = (b5 << 3U) | (b5 >> 2U);
    return (r8 << 16U) | (g8 << 8U) | b8;
}

bool bsp_lcd_fill_async(uint16_t colour, void (*callback)(void *), void *user_data)
{
    if (hdma2d.State != HAL_DMA2D_STATE_READY)
    {
        return false;
    }
    if (callback == NULL)
    {
        return false;
    }
    fill_callback            = callback;
    fill_user_data           = user_data;
    hdma2d.XferCpltCallback  = dma2d_fill_cplt_shim;
    hdma2d.XferErrorCallback = dma2d_fill_cplt_shim;

    if (HAL_DMA2D_Start_IT(&hdma2d,
                           rgb565_to_argb8888(colour),
                           (uint32_t)bsp_lcd_framebuffer(),
                           BSP_LCD_WIDTH,
                           BSP_LCD_HEIGHT) != HAL_OK)
    {
        fill_callback  = NULL;
        fill_user_data = NULL;
        return false;
    }
    return true;
}

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return FRAMEBUFFER;
}

bool bsp_lcd_fill_rect_async(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             uint16_t colour, void (*callback)(void *), void *user_data)
{
    if (hdma2d.State != HAL_DMA2D_STATE_READY || callback == NULL || w == 0 || h == 0)
    {
        return false;
    }
    if (x >= BSP_LCD_WIDTH || y >= BSP_LCD_HEIGHT)
    {
        return false;
    }
    if (x + w > BSP_LCD_WIDTH)
    {
        w = (uint16_t)(BSP_LCD_WIDTH - x);
    }
    if (y + h > BSP_LCD_HEIGHT)
    {
        h = (uint16_t)(BSP_LCD_HEIGHT - y);
    }
    fill_callback       = callback;
    fill_user_data     = user_data;
    rect_restore_offset = true;
    hdma2d.XferCpltCallback  = dma2d_fill_cplt_shim;
    hdma2d.XferErrorCallback = dma2d_fill_cplt_shim;
    hdma2d.Init.OutputOffset = BSP_LCD_WIDTH - w;
    volatile uint16_t *fb = bsp_lcd_framebuffer();
    uint32_t dst = (uint32_t)(fb + (uint32_t)y * BSP_LCD_WIDTH + x);
    if (HAL_DMA2D_Start_IT(&hdma2d,
                           rgb565_to_argb8888(colour),
                           dst,
                           w,
                           h) != HAL_OK)
    {
        hdma2d.Init.OutputOffset = 0;
        rect_restore_offset      = false;
        fill_callback            = NULL;
        fill_user_data           = NULL;
        return false;
    }
    return true;
}

#else

void bsp_lcd_release_reset(void) {}
void bsp_lcd_enable(void) {}
void bsp_lcd_backlight_on(void) {}
void bsp_lcd_backlight_off(void) {}

bool bsp_lcd_fill_async(uint16_t colour, void (*callback)(void *), void *user_data)
{
    (void)colour;
    (void)callback;
    (void)user_data;
    return false;
}

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return NULL;
}

bool bsp_lcd_fill_rect_async(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             uint16_t colour, void (*callback)(void *), void *user_data)
{
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)colour;
    (void)callback;
    (void)user_data;
    return false;
}

#endif /* CORE_CM4 */
