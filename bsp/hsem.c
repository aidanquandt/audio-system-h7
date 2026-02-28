#include "bsp/hsem.h"
#include "stm32h7xx_hal.h"

#define HSEM_NUM_CHANNELS 32U

static bsp_hsem_callback_t hsem_callbacks[HSEM_NUM_CHANNELS];

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

void bsp_hsem_register_callback(uint32_t channel_mask, bsp_hsem_callback_t cb)
{
    for (uint32_t i = 0; i < HSEM_NUM_CHANNELS; i++) {
        if (channel_mask & (1U << i)) {
            hsem_callbacks[i] = cb;
        }
    }
}

void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
    /* Re-arm before dispatching so no notification is lost if the callback
       itself triggers another release on the same channel. */
    HAL_HSEM_ActivateNotification(SemMask);

    uint32_t mask = SemMask;
    while (mask) {
        uint32_t i = (uint32_t)__builtin_ctz(mask);
        if (hsem_callbacks[i]) {
            hsem_callbacks[i](1U << i);
        }
        mask &= mask - 1U;  /* clear lowest set bit */
    }
}
