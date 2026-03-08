#include "drivers/touch/touch.h"

#ifdef CORE_CM4

#include "drivers/i2c/i2c.h"
#include "drivers/lcd/lcd.h"
#include "FreeRTOS.h"
#include "task.h"

/* GT911 on STM32H745I-DISCO: I2C 7-bit addr 0x5D (0xBA >> 1). */
#define GT911_ADDR 0x5DU

/* GT911 registers (16-bit). */
#define GT911_REG_PID       0x8140U
#define GT911_REG_BUF_STAT  0x814EU  /* bit 7 = buffer ready, bits 3:0 = point count */
#define GT911_REG_P1_DATA   0x814FU  /* first touch point: 8 bytes (TrackID, X_L,X_H, Y_L,Y_H, Area, reserved) */
/* Panel reports in display resolution; scale using LCD_DRIVER_* as single source of truth. */

static bool s_gt911_ok = false;
static touch_state_t s_last = {0, 0, false};

static bool touch_driver_probe_gt911(void)
{
    uint8_t id[4];
    if (!i2c_driver_read16(GT911_ADDR, GT911_REG_PID, id, sizeof(id)))
    {
        return false;
    }
    return id[0] == '9' && id[1] == '1' && id[2] == '1';
}

/**
 * Clear buffer status (0x814E) so the IC can set bit 7 on next touch.
 * Required after power-up / probe; some boards need this before touches are reported.
 */
static void touch_driver_gt911_clear_buffer(void)
{
    uint8_t zero = 0;
    for (int i = 0; i < 3; i++)
    {
        (void)i2c_driver_write16(GT911_ADDR, GT911_REG_BUF_STAT, &zero, 1);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static bool touch_driver_read_gt911(touch_state_t *out)
{
    /* 0x814E: one byte, bit 7 = new data, bits 3:0 = point count.
     * Read 2 bytes in one transaction; some GT911 variants behave better this way. */
    uint8_t buf[2];
    if (!i2c_driver_read16(GT911_ADDR, GT911_REG_BUF_STAT, buf, 2))
    {
        return false;
    }
    uint8_t status = buf[0];
    if ((status & 0x80U) == 0)
    {
        out->pressed = false;
        return true;
    }
    uint8_t count = status & 0x0FU;
    if (count == 0)
    {
        out->pressed = false;
        touch_driver_gt911_clear_buffer();
        return true;
    }
    /* First point at 0x814F: 8 bytes = TrackID(1), X_lo,X_hi(2), Y_lo,Y_hi(2), Area(2), reserved.
     * Byte order from GT911: buf[0]=TrackID, buf[1..2]=X LE, buf[3..4]=Y LE. */
    uint8_t pt[8];
    if (!i2c_driver_read16(GT911_ADDR, GT911_REG_P1_DATA, pt, 8))
    {
        return false;
    }
    uint16_t raw_x = (uint16_t)pt[1] | ((uint16_t)pt[2] << 8U);
    uint16_t raw_y = (uint16_t)pt[3] | ((uint16_t)pt[4] << 8U);
    /* Panel reports in display resolution; use LCD_DRIVER_* dimensions as single source of truth. */
    out->x = raw_x >= LCD_DRIVER_WIDTH ? LCD_DRIVER_WIDTH - 1U : raw_x;
    out->y = raw_y >= LCD_DRIVER_HEIGHT ? LCD_DRIVER_HEIGHT - 1U : raw_y;
    out->pressed = true;
    touch_driver_gt911_clear_buffer();
    return true;
}

void touch_driver_init(void)
{
    i2c_driver_init();
    vTaskDelay(pdMS_TO_TICKS(10));
    if (!touch_driver_probe_gt911())
    {
        return;
    }
    s_gt911_ok = true;
    /* Clear buffer so first touch sets bit 7; many boards never report until we do this. */
    touch_driver_gt911_clear_buffer();
    /* Give the IC time to be ready for the next touch (datasheet suggests ~50ms after config). */
    vTaskDelay(pdMS_TO_TICKS(50));
}

bool touch_driver_read(touch_state_t *out)
{
    if (out == NULL || !s_gt911_ok)
    {
        return false;
    }
    touch_state_t t = {0, 0, false};
    bool ok = touch_driver_read_gt911(&t);
    if (ok)
    {
        s_last = t;
        *out  = t;
    }
    else
    {
        *out = s_last;
    }
    return ok;
}

#else

void touch_driver_init(void)
{
}

bool touch_driver_read(touch_state_t *out)
{
    (void)out;
    return false;
}

#endif /* CORE_CM4 */
