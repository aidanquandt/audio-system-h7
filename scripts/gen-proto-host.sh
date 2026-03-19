#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PROTO="$PROJECT_ROOT/messaging_protocol"
OUT="$PROJECT_ROOT/host/src/host/generated"

mkdir -p "$OUT"
protoc --python_out="$OUT" \
  -I "$PROTO" \
  "$PROTO/messaging_protocol.proto"
