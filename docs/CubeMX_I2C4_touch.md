# CubeMX: I2C4 for touch (STM32H745I-DISCO)

On **STM32H745I-DISCO**, the LCD capacitive touch controller is connected over **I2C4** with the following pinout (from the official BSP).

## I2C port and pins

| Role | Pin  | Alternate function |
|------|------|--------------------|
| **I2C4 SCL** | **PD12** | GPIO_AF4_I2C4 |
| **I2C4 SDA** | **PD13** | GPIO_AF4_I2C4 |
| Touch interrupt | PG2 | Already in project as `LCD_INT_Pin` |

Use **I2C4** only. PD12 and PD13 are not used by FMC in your current project, so they are free for I2C4.

---

## CubeMX steps

1. **Open** `cube/cube.ioc` in STM32CubeMX.

2. **Pinout & Configuration**
   - In the pinout view, find **PD12** and **PD13**.
   - Click **PD12** → set to **I2C4_SCL**.
   - Click **PD13** → set to **I2C4_SDA**.
   - If CubeMX asks to enable I2C4, confirm.

3. **I2C4 configuration**
   - In the left tree: **Connectivity** → **I2C4**.
   - **Mode**: I2C.
   - **Configuration** (Parameter Settings):
     - **I2C Speed Mode**: Standard Mode (100 kHz) or Fast Mode (400 kHz).  
       The official BSP uses 100 kHz; 400 kHz is fine for the touch controller.
     - Leave other options at default (e.g. no clock stretch, standard timing).

4. **NVIC (optional, for later)**
   - If you use I2C interrupts later, enable **Connectivity** → **I2C4** → **NVIC** and enable the I2C4 event interrupt.  
   Not required for a simple blocking/polling touch driver.

5. **Generate code**
   - **Project** → **Generate Code** (or the toolbar button).
   - Keep your code generator settings (e.g. CM4/CM7, HAL, existing paths).

After generation you should have:
- `cube/CM4/Core/Src/i2c.c` (or similar) with `MX_I2C4_Init()` and `hi2c4`.
- `cube/CM4/Core/Inc/main.h` (or the i2c header) with `hi2c4` declared.
- PD12/PD13 configured as I2C4_SCL/SDA in `gpio.c`.

---

## Touch controller details (for your driver)

- **Possible ICs** on this board: **FT5336** (7‑bit address **0x70**) or **GT911** (address **0xBA** or **0x28**). The BSP probes GT911 first, then FT5336.
- **Interrupt**: PG2 is already configured as `LCD_INT_Pin` (rising edge in your `gpio.c`). The touch driver can use this for “touch event” (e.g. give a semaphore and read coordinates in a task).

---

## Summary

| Item        | Value                          |
|------------|---------------------------------|
| I2C port   | **I2C4**                        |
| SCL        | **PD12**, AF4                   |
| SDA        | **PD13**, AF4                   |
| Speed      | 100 kHz (Standard) or 400 kHz  |
| Touch INT  | PG2 (already LCD_INT)           |

Configure I2C4 and PD12/PD13 in CubeMX as above, then generate code. After that you can add `bsp/i2c` and `drivers/touch` that use `hi2c4` and the touch IC addresses.
