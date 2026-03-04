#!/bin/bash
# Regenerate RPC message definitions from proto/messages.yaml

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "=== Running RPC code generation ==="
echo

python3 "$PROJECT_ROOT/tools/codegen/generate.py"

echo
echo "✓ Codegen complete"
echo "  Generated files:"
echo "    - include/messages.h"
echo "    - generated/rpc.h"
echo "    - generated/rpc.c"
echo "    - tools/host/rpc_messages.py"
