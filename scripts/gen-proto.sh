#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
NANOPB="$PROJECT_ROOT/third_party/nanopb"
PROTO="$PROJECT_ROOT/messaging_protocol"
OUT="$PROJECT_ROOT/generated/messaging_protocol"

mkdir -p "$OUT"

NANOPB_GEN="$NANOPB/generator/nanopb_generator.py"
if [[ ! -f "$NANOPB_GEN" ]]; then
  echo "gen-proto: nanopb generator missing: $NANOPB_GEN" >&2
  echo "Initialize the submodule: git submodule update --init --recursive" >&2
  exit 1
fi

# -D = output dir, -I = search path for .options files (same dir as proto)
python3 "$NANOPB_GEN" \
  -D "$OUT" \
  -I "$PROTO" \
  "$PROTO/messaging_protocol.proto"
