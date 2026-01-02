#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../analytics/utils/common.sh"

# Create a cross builddir under bin/build-win
ROOT_DIR="$(go_to_project_root)"
BUILD_WIN="$ROOT_DIR/bin/build-win"
CROSS_FILE="$ROOT_DIR/bin/export/cross-windows.ini"

meson_setup_or_reconfigure "release" "win" "$CROSS_FILE"

meson_compile_target "release" "build_native_archive" "win"
