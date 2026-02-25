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
- `src/` - Application code
- `include/` - Headers
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
