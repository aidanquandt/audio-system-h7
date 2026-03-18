#!/bin/bash
# Clean, configure, and build both cores

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== Full Rebuild ==="
echo

"$SCRIPT_DIR/clean.sh"
echo
"$SCRIPT_DIR/configure.sh"
echo
"$SCRIPT_DIR/build.sh"
