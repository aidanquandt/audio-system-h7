#!/bin/bash
# Configure both CM4 and CM7 cores

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=== Configuring STM32H745 Dual-Core Project ==="
echo

# Configure CM4
echo "Configuring CM4..."
cmake -S "$PROJECT_ROOT/cube/CM4" \
      -B "$PROJECT_ROOT/build/CM4" \
      --preset Debug

echo

# Configure CM7
echo "Configuring CM7..."
cmake -S "$PROJECT_ROOT/cube/CM7" \
      -B "$PROJECT_ROOT/build/CM7" \
      --preset Debug

echo
echo "✓ Configuration complete"
echo "  Build outputs:"
echo "    - build/CM4/"
echo "    - build/CM7/"
