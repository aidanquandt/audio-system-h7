#!/bin/bash
# Fix CubeMX-generated SystemClock_Config for 480 MHz operation.
# CubeMX bug: generates VOS2 + FLASH_LATENCY_0 instead of VOS0 + LATENCY_4.
#
# Run this after every CubeMX code generation, or call it from build.sh.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CM7_MAIN="$PROJECT_ROOT/cube/CM7/Core/Src/main.c"

if [[ ! -f "$CM7_MAIN" ]]; then
    echo "Error: $CM7_MAIN not found"
    exit 1
fi

CHANGED=0

# Fix 1: VOS2 → VOS1 + VOS0 overdrive sequence
if grep -q 'PWR_REGULATOR_VOLTAGE_SCALE2' "$CM7_MAIN"; then
    sed -i \
        's|__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);|/* CubeMX fix: VOS2 is wrong for 480 MHz. Use VOS1 then overdrive to VOS0. */\n  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);|' \
        "$CM7_MAIN"

    # Insert VOS0 overdrive call after the VOSRDY wait loop
    sed -i \
        '/while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}/a\\n  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0);' \
        "$CM7_MAIN"

    CHANGED=1
    echo "✓ Fixed voltage scaling: VOS2 → VOS1 + VOS0 overdrive"
fi

# Fix 2: FLASH_LATENCY_0 → FLASH_LATENCY_4
if grep -q 'HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)' "$CM7_MAIN"; then
    sed -i \
        's|HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)|HAL_RCC_ClockConfig(\&RCC_ClkInitStruct, FLASH_LATENCY_4)|' \
        "$CM7_MAIN"

    CHANGED=1
    echo "✓ Fixed flash latency: 0 → 4 wait states"
fi

if [[ $CHANGED -eq 0 ]]; then
    echo "  No CubeMX clock fixes needed (already patched)"
else
    echo "  Applied CubeMX clock fixes to CM7 main.c"
fi
