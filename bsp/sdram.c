#include "bsp/sdram.h"

#ifdef CORE_CM4

#include "fmc.h"
#include "main.h"

#define BSP_SDRAM_BASE   ((volatile uint32_t *)0xD0000000UL)

static void send_cmd(uint8_t mode, uint8_t target, uint8_t auto_refresh_num, uint32_t mode_reg)
{
    FMC_SDRAM_CommandTypeDef cmd = {0};
    cmd.CommandMode            = mode;
    cmd.CommandTarget          = target;
    cmd.AutoRefreshNumber      = auto_refresh_num;
    cmd.ModeRegisterDefinition = mode_reg;
    HAL_SDRAM_SendCommand(&hsdram2, &cmd, 0xFFFF);
}

void bsp_sdram_cmd_clock_enable(void)
{
    send_cmd(FMC_SDRAM_CMD_CLK_ENABLE, FMC_SDRAM_CMD_TARGET_BANK2, 1, 0);
}

void bsp_sdram_cmd_precharge_all(void)
{
    send_cmd(FMC_SDRAM_CMD_PALL, FMC_SDRAM_CMD_TARGET_BANK2, 1, 0);
}

void bsp_sdram_cmd_auto_refresh(uint8_t count)
{
    send_cmd(FMC_SDRAM_CMD_AUTOREFRESH_MODE, FMC_SDRAM_CMD_TARGET_BANK2, count, 0);
}

void bsp_sdram_cmd_load_mode(uint32_t mode_reg)
{
    send_cmd(FMC_SDRAM_CMD_LOAD_MODE, FMC_SDRAM_CMD_TARGET_BANK2, 1, mode_reg);
}

void bsp_sdram_set_refresh_rate(uint32_t count)
{
    HAL_SDRAM_ProgramRefreshRate(&hsdram2, count);
}

bool bsp_sdram_test(void)
{
    volatile uint32_t *sdram = BSP_SDRAM_BASE;
    for (uint32_t i = 0; i < 256; i++) {
        sdram[i] = 0xA5A50000UL | i;
    }
    for (uint32_t i = 0; i < 256; i++) {
        if (sdram[i] != (0xA5A50000UL | i)) {
            return false;
        }
    }
    return true;
}

#else

void bsp_sdram_cmd_clock_enable(void) { (void)0; }
void bsp_sdram_cmd_precharge_all(void) { (void)0; }
void bsp_sdram_cmd_auto_refresh(uint8_t count) { (void)count; }
void bsp_sdram_cmd_load_mode(uint32_t mode_reg) { (void)mode_reg; }
void bsp_sdram_set_refresh_rate(uint32_t count) { (void)count; }
bool bsp_sdram_test(void) { return true; }

#endif /* CORE_CM4 */
