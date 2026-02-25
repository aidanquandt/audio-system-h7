#!/bin/bash
# Flash script for STM32H745

ELF_FILE="../build/cube.elf"

if [ ! -f "$ELF_FILE" ]; then
    echo "Error: Build file not found: $ELF_FILE"
    exit 1
fi

# Using STM32CubeProgrammer CLI
# STM32_Programmer_CLI -c port=SWD -w $ELF_FILE -v -rst

# Or using OpenOCD
openocd -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg \
    -c "program $ELF_FILE verify reset exit"
