# STM32H745 Audio System

Dual-core audio/DSP firmware for STM32H745-DISCO board.

## Quick Start

```bash
./scripts/build.sh    # Build both CM4 and CM7
./scripts/flash.sh    # Flash to board
```

Press `Ctrl+Shift+B` in VSCode to build.

## Project Layout

- `cube/` - STM32CubeMX generated code (don't edit directly)
- `src/` - Application code (cm4/, cm7/, common/)
- `drivers/` - IC/component drivers (board-agnostic, I/O injected)
- `bsp/` - Board support package (glues drivers to HAL)
- `lib/` - Third-party submodules (lvgl, cmsis-dsp, fatfs)
- `include/` - Cross-core shared public headers (IPC structs, shared memory layout)
- `scripts/` - Build scripts
- `build/` - Build output

## Build Scripts

- `build.sh` - Build both cores
- `flash.sh` - Flash firmware
- `clean.sh` - Clean build
- `rebuild.sh` - Clean + build

## Hardware

STM32H745I-DISCO  
- CM7 @ 480MHz - Main DSP processing
- CM4 @ 240MHz - Peripherals and I/O

## Known Issues / TODO

- **GDB path**: `launch.json` has `gdbPath: "gdb-multiarch"` which is Linux-only. On Windows with the ARM toolchain this should be `arm-none-eabi-gdb` (or full path).
- **SVD file**: `launch.json` has `svdFile: ""` — no peripheral register view in debugger. Add the STM32H745 SVD from STM32CubeH7 or https://github.com/STMicroelectronics/cmsis-header-stm32.
- **Missing Attach CM4 debug config**: `launch.json` has Attach CM7 but no Attach CM4 configuration.
- **OpenOCD CM4 reset event**: `openocd.cfg` only sets adapter speed for `cpu0` (CM7). Add a matching `reset-init` event for `cpu1` (CM4).
