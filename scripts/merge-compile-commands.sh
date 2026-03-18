#!/bin/bash
# Merge compile_commands.json from both cores for IntelliSense

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CM4_JSON="$PROJECT_ROOT/build/CM4/compile_commands.json"
CM7_JSON="$PROJECT_ROOT/build/CM7/compile_commands.json"
OUTPUT_JSON="$PROJECT_ROOT/build/compile_commands.json"

# Check if both files exist
if [[ ! -f "$CM4_JSON" ]] || [[ ! -f "$CM7_JSON" ]]; then
    exit 0  # Silently skip if not configured yet
fi

# Merge using jq if available, otherwise fallback
if command -v jq &> /dev/null; then
    jq -s 'add' "$CM4_JSON" "$CM7_JSON" > "$OUTPUT_JSON" 2>/dev/null
else
    # Fallback: use python for a robust JSON merge
    python3 - "$CM4_JSON" "$CM7_JSON" "$OUTPUT_JSON" <<'PY'
import json, sys
cm4_path, cm7_path, out_path = sys.argv[1:]
with open(cm4_path, "r", encoding="utf-8") as f:
    cm4 = json.load(f)
with open(cm7_path, "r", encoding="utf-8") as f:
    cm7 = json.load(f)
merged = cm4 + cm7
with open(out_path, "w", encoding="utf-8") as f:
    json.dump(merged, f)
PY
fi
