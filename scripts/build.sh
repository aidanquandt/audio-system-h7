#!/bin/bash
# Build both CM4 and CM7 cores

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Number of parallel jobs
JOBS="${JOBS:-8}"

# Auto-configure if build directories don't exist
if [[ ! -d "$PROJECT_ROOT/build/CM4" ]] || [[ ! -d "$PROJECT_ROOT/build/CM7" ]]; then
    echo "Build directories not found. Configuring..."
    "$SCRIPT_DIR/configure.sh"
    echo
fi

echo "=== Building STM32H745 Dual-Core Project ==="
echo

# Apply CubeMX clock fixes (VOS0 + flash latency for 480 MHz)
"$SCRIPT_DIR/fix-cubemx-clocks.sh"
echo

# Build CM4
echo "Building CM4..."
cmake --build "$PROJECT_ROOT/build/CM4" -j"$JOBS"

echo

# Build CM7
echo "Building CM7..."
cmake --build "$PROJECT_ROOT/build/CM7" -j"$JOBS"

echo
echo "✓ Build complete"
echo "  Outputs:"
echo "    - build/CM4/cube_CM4.elf"
echo "    - build/CM7/cube_CM7.elf"

# Update IntelliSense
"$SCRIPT_DIR/merge-compile-commands.sh"
