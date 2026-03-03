#include "bsp/sdram.h"

#ifdef CORE_CM4

#include "fmc.h"
#include "main.h"

#define BSP_SDRAM_BASE     ((volatile uint32_t *)0xD0000000UL)
#define SDRAM_MODE_REG     0x0230U   /* burst 1, sequential, CAS 3, write single */
#define SDRAM_REFRESH      1542U     /* 100 MHz clock, 4096 rows, 64 ms interval */

static void send_cmd(uint8_t mode, uint8_t target, uint8_t auto_refresh_num, uint32_t mode_reg)
{
    FMC_SDRAM_CommandTypeDef cmd = {0};
    cmd.CommandMode            = mode;
    cmd.CommandTarget          = target;
    cmd.AutoRefreshNumber      = auto_refresh_num;
    cmd.ModeRegisterDefinition = mode_reg;
    HAL_SDRAM_SendCommand(&hsdram2, &cmd, 0xFFFF);
}

void bsp_sdram_enable_clock(void)
{
    send_cmd(FMC_SDRAM_CMD_CLK_ENABLE, FMC_SDRAM_CMD_TARGET_BANK2, 1, 0);
}

void bsp_sdram_configure(void)
{
    send_cmd(FMC_SDRAM_CMD_PALL,            FMC_SDRAM_CMD_TARGET_BANK2, 1, 0);
    send_cmd(FMC_SDRAM_CMD_AUTOREFRESH_MODE, FMC_SDRAM_CMD_TARGET_BANK2, 2, 0);
    send_cmd(FMC_SDRAM_CMD_LOAD_MODE,       FMC_SDRAM_CMD_TARGET_BANK2, 1, SDRAM_MODE_REG);
    HAL_SDRAM_ProgramRefreshRate(&hsdram2, SDRAM_REFRESH);
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

void bsp_sdram_enable_clock(void) {}
void bsp_sdram_configure(void)    {}
bool bsp_sdram_test(void)         { return true; }

#endif /* CORE_CM4 */
