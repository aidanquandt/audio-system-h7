#!/bin/bash
# Clean build artifacts

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=== Cleaning build artifacts ==="
rm -rf "$PROJECT_ROOT/build"
echo "✓ Clean complete"
