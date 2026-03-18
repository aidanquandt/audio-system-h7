#!/bin/bash
# Flash both CM4 and CM7 cores to STM32H745

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Change to project root for relative paths
cd "$PROJECT_ROOT"

CM4_ELF="build/CM4/stm32_hw_CM4.elf"
CM7_ELF="build/CM7/stm32_hw_CM7.elf"

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

OPENOCD_CFG="$PROJECT_ROOT/tools/openocd/openocd.cfg"

if command -v openocd &> /dev/null; then
    openocd -f "$OPENOCD_CFG" \
        -c "program $CM7_ELF verify" \
        -c "program $CM4_ELF verify" \
        -c "reset run" \
        -c "exit"

    echo
    echo "✓ Flash complete"
else
    echo "Error: OpenOCD not found!"
    echo "  Linux/Dev Container: apt install openocd"
    echo "  Windows (MSYS2):    pacman -S mingw-w64-ucrt-x86_64-openocd"
    exit 1
fi
