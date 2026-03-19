#!/bin/bash
# Flash both CM4 and CM7 cores to STM32H745 using combined dualcore.hex

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Change to project root for relative paths
cd "$PROJECT_ROOT"

# Ensure dualcore.hex exists (build.sh runs make-dualcore-hex.sh)
"$SCRIPT_DIR/make-dualcore-hex.sh" >/dev/null 2>&1 || true

DUALCORE_HEX="build/dualcore.hex"

if [[ ! -f "$DUALCORE_HEX" ]]; then
    echo "Error: Dual-core firmware not found: $DUALCORE_HEX"
    echo "Run: make build"
    exit 1
fi

echo "=== Flashing STM32H745 Dual-Core ==="
echo

"$SCRIPT_DIR/attach-usb.sh"

OPENOCD_CFG="$PROJECT_ROOT/tools/openocd/openocd.cfg"

if command -v openocd &> /dev/null; then
    openocd -f "$OPENOCD_CFG" \
        -c "program $DUALCORE_HEX verify" \
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
