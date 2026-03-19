#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PROTO="$PROJECT_ROOT/messaging_protocol"
OUT="$PROJECT_ROOT/host/src/host/generated"

mkdir -p "$OUT"
# Use grpc_tools.protoc for protobuf 4.x compatibility (matches nanopb generator)
python3 -m grpc_tools.protoc --python_out="$OUT" \
  -I "$PROTO" \
  "$PROTO/messaging_protocol.proto"
