#include "bsp/lcd/lcd.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef CORE_CM4

#include "dma2d.h"
#include "main.h"
#include "stm32h7xx_hal_dma2d.h"

/* LTDC layer 0 framebuffer in SDRAM (address from Cube/LTDC config). */
#define FRAMEBUFFER ((volatile uint16_t *)0xD0000000UL)

static void (*xfer_callback)(void *) = NULL;
static void    *xfer_user_data       = NULL;
static uint32_t r2m_restore_oor      = 0U;
static uint32_t current_mode         = DMA2D_R2M;

static void bsp_lcd_xfer_done_cb(DMA2D_HandleTypeDef *hdma2d);

static void set_xfer_callback(void (*cb)(void *), void *ud)
{
    xfer_callback            = cb;
    xfer_user_data           = ud;
    hdma2d.XferCpltCallback  = bsp_lcd_xfer_done_cb;
    hdma2d.XferErrorCallback = bsp_lcd_xfer_done_cb;
}

static void clear_xfer_callback(void)
{
    xfer_callback  = NULL;
    xfer_user_data = NULL;
}

/* Clamp *w, *h to fit at (x, y). Returns false if out of bounds or zero area. */
static bool clamp_rect(uint16_t x, uint16_t y, uint16_t *w, uint16_t *h)
{
    if (x >= BSP_LCD_WIDTH || y >= BSP_LCD_HEIGHT)
    {
        return false;
    }
    if (x + *w > BSP_LCD_WIDTH)
    {
        *w = (uint16_t)(BSP_LCD_WIDTH - x);
    }
    if (y + *h > BSP_LCD_HEIGHT)
    {
        *h = (uint16_t)(BSP_LCD_HEIGHT - y);
    }
    return (*w != 0U && *h != 0U);
}

static void set_output_offset(uint32_t out_off)
{
    hdma2d.Init.OutputOffset = out_off;
    hdma2d.Instance->OOR     = out_off;
}

static void set_layer_rgb565(DMA2D_LayerCfgTypeDef *layer, uint32_t input_offset)
{
    layer->InputOffset    = input_offset;
    layer->InputColorMode = DMA2D_INPUT_RGB565;
    layer->AlphaMode      = DMA2D_NO_MODIF_ALPHA;
    layer->InputAlpha     = 0xFFU;
    layer->AlphaInverted  = DMA2D_REGULAR_ALPHA;
    layer->RedBlueSwap    = DMA2D_RB_REGULAR;
}

static void bsp_lcd_xfer_done_cb(DMA2D_HandleTypeDef *hdma2d)
{
    (void)hdma2d;
    if (r2m_restore_oor != 0U)
    {
        hdma2d->Init.OutputOffset = 0;
        hdma2d->Instance->OOR     = 0U;
        r2m_restore_oor           = 0U;
    }
    if (xfer_callback != NULL)
    {
        void (*cb)(void *) = xfer_callback;
        void *ud           = xfer_user_data;
        xfer_callback      = NULL;
        xfer_user_data     = NULL;
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

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return FRAMEBUFFER;
}

/* R2M: HAL expects 32-bit ARGB8888 colour; output is RGB565. */
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

bool bsp_lcd_fill_rect_async(uint16_t x,
                             uint16_t y,
                             uint16_t w,
                             uint16_t h,
                             uint16_t colour,
                             void (*callback)(void *),
                             void *user_data)
{
    if (hdma2d.State != HAL_DMA2D_STATE_READY || callback == NULL)
    {
        return false;
    }
    if (!clamp_rect(x, y, &w, &h))
    {
        return false;
    }

    r2m_restore_oor = 0U;
    set_xfer_callback(callback, user_data);

    uint32_t out_off =
        (w < BSP_LCD_WIDTH || h < BSP_LCD_HEIGHT) ? (uint32_t)(BSP_LCD_WIDTH - w) : 0U;
    if (current_mode != DMA2D_R2M)
    {
        hdma2d.Init.Mode         = DMA2D_R2M;
        hdma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
        hdma2d.Init.OutputOffset = out_off;
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
        {
            clear_xfer_callback();
            return false;
        }
        current_mode = DMA2D_R2M;
    }
    set_output_offset(out_off);
    if (out_off != 0U)
    {
        r2m_restore_oor = out_off;
    }

    uint32_t dst = (uint32_t)(bsp_lcd_framebuffer() + (uint32_t)y * BSP_LCD_WIDTH + x);
    if (HAL_DMA2D_Start_IT(&hdma2d, rgb565_to_argb8888(colour), dst, w, h) != HAL_OK)
    {
        set_output_offset(0);
        r2m_restore_oor = 0U;
        clear_xfer_callback();
        return false;
    }
    return true;
}

bool bsp_lcd_copy_rect_async(const uint16_t *src,
                             uint32_t        src_stride,
                             uint16_t        dst_x,
                             uint16_t        dst_y,
                             uint16_t        w,
                             uint16_t        h,
                             void (*callback)(void *),
                             void *user_data)
{
    if (hdma2d.State != HAL_DMA2D_STATE_READY || callback == NULL || src == NULL)
    {
        return false;
    }
    if (src_stride < w)
    {
        src_stride = w;
    }
    if (!clamp_rect(dst_x, dst_y, &w, &h))
    {
        return false;
    }

    set_xfer_callback(callback, user_data);

    uint32_t out_off = (uint32_t)(BSP_LCD_WIDTH - w);
    if (current_mode != DMA2D_M2M)
    {
        hdma2d.Init.Mode         = DMA2D_M2M;
        hdma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
        hdma2d.Init.OutputOffset = out_off;
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
        {
            clear_xfer_callback();
            return false;
        }
        set_layer_rgb565(&hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER], src_stride - w);
        if (HAL_DMA2D_ConfigLayer(&hdma2d, DMA2D_FOREGROUND_LAYER) != HAL_OK)
        {
            clear_xfer_callback();
            return false;
        }
        current_mode = DMA2D_M2M;
    }
    else
    {
        set_layer_rgb565(&hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER], src_stride - w);
        (void)HAL_DMA2D_ConfigLayer(&hdma2d, DMA2D_FOREGROUND_LAYER);
    }
    set_output_offset(out_off);

    uint32_t dst = (uint32_t)(bsp_lcd_framebuffer() + (uint32_t)dst_y * BSP_LCD_WIDTH + dst_x);
    if (HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)src, dst, w, h) != HAL_OK)
    {
        clear_xfer_callback();
        return false;
    }
    return true;
}

#else

void bsp_lcd_release_reset(void) {}
void bsp_lcd_enable(void) {}
void bsp_lcd_backlight_on(void) {}
void bsp_lcd_backlight_off(void) {}

volatile uint16_t *bsp_lcd_framebuffer(void)
{
    return NULL;
}

bool bsp_lcd_fill_rect_async(uint16_t x,
                             uint16_t y,
                             uint16_t w,
                             uint16_t h,
                             uint16_t colour,
                             void (*callback)(void *),
                             void *user_data)
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

bool bsp_lcd_copy_rect_async(const uint16_t *src,
                             uint32_t        src_stride,
                             uint16_t        dst_x,
                             uint16_t        dst_y,
                             uint16_t        w,
                             uint16_t        h,
                             void (*callback)(void *),
                             void *user_data)
{
    (void)src;
    (void)src_stride;
    (void)dst_x;
    (void)dst_y;
    (void)w;
    (void)h;
    (void)callback;
    (void)user_data;
    return false;
}

#endif /* CORE_CM4 */
