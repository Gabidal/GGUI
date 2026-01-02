#!/usr/bin/env bash
set -euo pipefail

# ----------------------------------------------------------------------------
# Expects project initialization by init.sh
# Runs meson compile [target(s)] -C [build type]
#
# Use as:
# build.sh profile ggui
# build.sh debug time2 timingStanding timingBusy
# ----------------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

build_type="${1:-}"

# Configure if needed
meson_setup_or_reconfigure "$build_type"

# Build
meson_compile_target "$build_type" "${@:2}"

echo "Build completed successfully"
