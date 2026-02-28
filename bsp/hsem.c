#include "bsp/hsem.h"
#include "stm32h7xx_hal.h"

static volatile bsp_hsem_callback_t s_callback;

void bsp_hsem_init(void)
{
    __HAL_RCC_HSEM_CLK_ENABLE();
    __HAL_RCC_D2SRAM3_CLK_ENABLE();
}

void bsp_hsem_notify(uint32_t channel)
{
    HAL_HSEM_FastTake(channel);
    HAL_HSEM_Release(channel, 0);
}

void bsp_hsem_arm(uint32_t channel_mask)
{
    HAL_HSEM_ActivateNotification(channel_mask);
}

void bsp_hsem_register_callback(bsp_hsem_callback_t cb)
{
    s_callback = cb;
}

void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
    if (s_callback) {
        HAL_HSEM_ActivateNotification(SemMask);
        s_callback(SemMask);
    }
}
