#!/bin/bash
# Merge compile_commands.json from both cores for IntelliSense

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
    # Fallback: simple concatenation
    echo "[" > "$OUTPUT_JSON"
    tail -n +2 "$CM4_JSON" | head -n -1 >> "$OUTPUT_JSON"
    echo "," >> "$OUTPUT_JSON"
    tail -n +2 "$CM7_JSON" | head -n -1 >> "$OUTPUT_JSON"
    echo "]" >> "$OUTPUT_JSON"
fi
