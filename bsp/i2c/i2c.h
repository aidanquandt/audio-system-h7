#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * I2C4 bus (touch controller on STM32H745I-DISCO).
 * CubeMX must call MX_I2C4_Init() before any bsp_i2c_* call (e.g. from main).
 */

/** Init is a no-op; I2C4 is initialised in main. Call once if you need a placeholder. */
void bsp_i2c_init(void);

/**
 * Read from an I2C device (8-bit register address).
 * @param addr 7-bit I2C device address (left-aligned; HAL will shift).
 * @param reg  Register address (8-bit).
 * @param buf  Buffer to fill.
 * @param len  Number of bytes to read.
 * @return true on success, false on HAL error or timeout.
 */
bool bsp_i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * Read from an I2C device (16-bit register address).
 * @param addr 7-bit I2C device address.
 * @param reg  Register address (16-bit).
 * @param buf  Buffer to fill.
 * @param len  Number of bytes to read.
 */
bool bsp_i2c_read16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);

/**
 * Write to an I2C device (8-bit register address).
 */
bool bsp_i2c_write(uint8_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);

/**
 * Write to an I2C device (16-bit register address).
 */
bool bsp_i2c_write16(uint8_t addr, uint16_t reg, const uint8_t *buf, uint16_t len);