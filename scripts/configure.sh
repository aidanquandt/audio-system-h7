#!/bin/bash
# Configure both CM4 and CM7 cores

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

PRESET="${PRESET:-Debug}"

echo "=== Configuring STM32H745 Dual-Core Project ==="
echo "Preset: $PRESET"
echo

# Configure CM4
echo "Configuring CM4..."
cmake -S "$PROJECT_ROOT/stm32_hw/CM4" \
      -B "$PROJECT_ROOT/build/CM4" \
      --preset "$PRESET"

echo

# Configure CM7
echo "Configuring CM7..."
cmake -S "$PROJECT_ROOT/stm32_hw/CM7" \
      -B "$PROJECT_ROOT/build/CM7" \
      --preset "$PRESET"

echo
echo "✓ Configuration complete"
echo "  Build outputs:"
echo "    - build/CM4/"
echo "    - build/CM7/"
