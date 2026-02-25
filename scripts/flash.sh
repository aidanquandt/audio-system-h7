#!/bin/bash
# Flash both CM4 and CM7 cores to STM32H745

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Change to project root for relative paths
cd "$PROJECT_ROOT"

CM4_ELF="build/CM4/cube_CM4.elf"
CM7_ELF="build/CM7/cube_CM7.elf"

# Check if both ELF files exist
if [[ ! -f "$CM4_ELF" ]]; then
    echo "Error: CM4 firmware not found: $CM4_ELF"
    echo "Run: ./scripts/build.sh"
    exit 1
fi

if [[ ! -f "$CM7_ELF" ]]; then
    echo "Error: CM7 firmware not found: $CM7_ELF"
    echo "Run: ./scripts/build.sh"
    exit 1
fi

echo "=== Flashing STM32H745 Dual-Core ==="
echo

if command -v openocd &> /dev/null; then
    openocd -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg \
        -c "program $CM7_ELF verify" \
        -c "program $CM4_ELF verify" \
        -c "reset run" \
        -c "exit"
    
    echo
    echo "✓ Flash complete"
else
    echo "Error: OpenOCD not found!"
    echo "Install: pacman -S mingw-w64-ucrt-x86_64-openocd"
    exit 1
fi
