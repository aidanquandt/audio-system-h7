#!/bin/bash
# Build both cores then flash if build succeeds

set -e  # Exit on error - flash will not run if build fails

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"$SCRIPT_DIR/build.sh"
"$SCRIPT_DIR/flash.sh"
