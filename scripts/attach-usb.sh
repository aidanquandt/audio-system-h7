#!/bin/bash
# Ensure ST-Link (or other USB device) is attached to WSL for flashing.
# Invokes ~/.config/wsl-usb/attach.sh if present (WSL2 + usbipd setup).
# No-op on native Linux or when wsl-usb is not installed.

set -euo pipefail

ATTACH_SCRIPT="${HOME}/.config/wsl-usb/attach.sh"

if [[ -f "$ATTACH_SCRIPT" ]]; then
    "$ATTACH_SCRIPT" stlink 2>/dev/null || true
fi
