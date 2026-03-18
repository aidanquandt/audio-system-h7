#!/bin/bash
# Generate a single HEX that contains both CM7 and CM4 images.
#
# Output:
#   build/dualcore.hex
#
# Notes:
# - Intel HEX supports multiple address ranges; we merge by concatenating while
#   removing intermediate EOF records.
# - This is meant for programming with STM32CubeProgrammer as "one file".

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CM4_ELF="$PROJECT_ROOT/build/CM4/stm32_hw_CM4.elf"
CM7_ELF="$PROJECT_ROOT/build/CM7/stm32_hw_CM7.elf"

OUT_DIR="$PROJECT_ROOT/build/images"
CM4_HEX="$OUT_DIR/stm32_hw_CM4.hex"
CM7_HEX="$OUT_DIR/stm32_hw_CM7.hex"
DUAL_HEX="$OUT_DIR/dualcore.hex"
DUAL_HEX_ROOT="$PROJECT_ROOT/build/dualcore.hex"
CM4_HEX_ROOT="$PROJECT_ROOT/build/stm32_hw_CM4.hex"
CM7_HEX_ROOT="$PROJECT_ROOT/build/stm32_hw_CM7.hex"

if [[ ! -f "$CM4_ELF" || ! -f "$CM7_ELF" ]]; then
    # Build may not have run yet; silently skip so build.sh can call us unconditionally.
    exit 0
fi

if ! command -v arm-none-eabi-objcopy >/dev/null 2>&1; then
    echo "Error: arm-none-eabi-objcopy not found in PATH."
    exit 1
fi

mkdir -p "$OUT_DIR"

echo "Generating HEX files..."
arm-none-eabi-objcopy -O ihex "$CM7_ELF" "$CM7_HEX"
arm-none-eabi-objcopy -O ihex "$CM4_ELF" "$CM4_HEX"

echo "Merging into: build/images/dualcore.hex"
{
    # Drop EOF from first file, then append second file as-is (includes final EOF).
    # EOF record is exactly ":00000001FF" (optionally with CR).
    sed '/^:00000001FF\r\?$/d' "$CM7_HEX"
    cat "$CM4_HEX"
} > "$DUAL_HEX"

echo "✓ Dual-core HEX ready"
echo "  - build/images/dualcore.hex"

# Convenience copy at build root for easy browsing in GUI tools
cp -f "$DUAL_HEX" "$DUAL_HEX_ROOT"

# Keep build/ root focused on top-level entrypoints.
rm -f "$CM4_HEX_ROOT" "$CM7_HEX_ROOT"
