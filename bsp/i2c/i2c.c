#include "bsp/i2c/i2c.h"

#ifdef CORE_CM4

#include "main.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c4;

#define I2C4_TIMEOUT_MS 10U

void bsp_i2c_init(void)
{
    /* I2C4 is initialised in main via MX_I2C4_Init(). */
}

bool bsp_i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (buf == NULL)
    {
        return false;
    }
    return HAL_I2C_Mem_Read(&hi2c4,
                            (uint16_t)addr << 1U,
                            (uint16_t)reg,
                            I2C_MEMADD_SIZE_8BIT,
                            buf,
                            len,
                            I2C4_TIMEOUT_MS) == HAL_OK;
}

bool bsp_i2c_read16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    if (buf == NULL)
    {
        return false;
    }
    return HAL_I2C_Mem_Read(&hi2c4,
                            (uint16_t)addr << 1U,
                            reg,
                            I2C_MEMADD_SIZE_16BIT,
                            buf,
                            len,
                            I2C4_TIMEOUT_MS) == HAL_OK;
}

bool bsp_i2c_write(uint8_t addr, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    if (buf == NULL)
    {
        return false;
    }
    return HAL_I2C_Mem_Write(&hi2c4,
                             (uint16_t)addr << 1U,
                             (uint16_t)reg,
                             I2C_MEMADD_SIZE_8BIT,
                             (uint8_t *)buf,
                             len,
                             I2C4_TIMEOUT_MS) == HAL_OK;
}

bool bsp_i2c_write16(uint8_t addr, uint16_t reg, const uint8_t *buf, uint16_t len)
{
    if (buf == NULL && len > 0)
    {
        return false;
    }
    return HAL_I2C_Mem_Write(&hi2c4,
                             (uint16_t)addr << 1U,
                             reg,
                             I2C_MEMADD_SIZE_16BIT,
                             (uint8_t *)buf,
                             len,
                             I2C4_TIMEOUT_MS) == HAL_OK;
}

#else

void bsp_i2c_init(void) {}

bool bsp_i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return false;
}

bool bsp_i2c_read16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return false;
}

bool bsp_i2c_write(uint8_t addr, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return false;
}

bool bsp_i2c_write16(uint8_t addr, uint16_t reg, const uint8_t *buf, uint16_t len)
{
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;
    return false;
}

#endif /* CORE_CM4 */
