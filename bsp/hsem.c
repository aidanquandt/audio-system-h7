#include "bsp/hsem.h"
#include "stm32h7xx_hal.h"

static bsp_hsem_callback_t s_callback;

void bsp_hsem_init(void)
{
    __HAL_RCC_HSEM_CLK_ENABLE();
    __HAL_RCC_D2SRAM3_CLK_ENABLE();

#ifdef CORE_CM7
    MPU_Region_InitTypeDef mpu = {
        .Enable           = MPU_REGION_ENABLE,
        .Number           = MPU_REGION_NUMBER7,
        .BaseAddress      = 0x30040000UL,
        .Size             = MPU_REGION_SIZE_32KB,
        .SubRegionDisable = 0x00,
        .TypeExtField     = MPU_TEX_LEVEL0,
        .AccessPermission = MPU_REGION_FULL_ACCESS,
        .DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE,
        .IsShareable      = MPU_ACCESS_SHAREABLE,
        .IsCacheable      = MPU_ACCESS_NOT_CACHEABLE,
        .IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE,
    };
    HAL_MPU_Disable();
    HAL_MPU_ConfigRegion(&mpu);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#endif
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
