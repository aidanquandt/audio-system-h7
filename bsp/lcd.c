#include "bsp/lcd.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef CORE_CM4

#include "dma2d.h"
#include "main.h"

#define FRAMEBUFFER ((volatile uint16_t *)0xD0000000UL)

static void (*fill_callback)(void *) = NULL;
static void *fill_user_data          = NULL;

static void dma2d_fill_cplt_shim(DMA2D_HandleTypeDef *hdma2d)
{
    (void)hdma2d;
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
                           (uint32_t)colour,
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

#endif /* CORE_CM4 */
