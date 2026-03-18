#!/bin/bash
# Build both CM4 and CM7 cores

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Number of parallel jobs (override: JOBS=16 ./scripts/build.sh)
if [[ -z "${JOBS:-}" ]]; then
    if command -v nproc >/dev/null 2>&1; then
        JOBS="$(nproc)"
    else
        JOBS="8"
    fi
fi

# Auto-configure if build directories don't exist
if [[ ! -d "$PROJECT_ROOT/build/CM4" ]] || [[ ! -d "$PROJECT_ROOT/build/CM7" ]]; then
    echo "Build directories not found. Configuring..."
    "$SCRIPT_DIR/configure.sh"
    echo
fi

echo "=== Building STM32H745 Dual-Core Project ==="
echo

# Build CM4
echo "Building CM4..."
cmake --build "$PROJECT_ROOT/build/CM4" --parallel "$JOBS"

echo

# Build CM7
echo "Building CM7..."
cmake --build "$PROJECT_ROOT/build/CM7" --parallel "$JOBS"

echo
echo "✓ Build complete"
echo "  Outputs:"
echo "    - build/CM4/stm32_hw_CM4.elf"
echo "    - build/CM7/stm32_hw_CM7.elf"

# Update IntelliSense
"$SCRIPT_DIR/merge-compile-commands.sh"

# Generate a single file to flash with STM32CubeProgrammer
"$SCRIPT_DIR/make-dualcore-hex.sh"
