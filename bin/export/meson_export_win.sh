#!/usr/bin/env bash
set -euo pipefail

# Source shared utilities for Meson helpers
UTILS_COMMON="$(cd "$(dirname "$0")/../analytics/utils" && pwd)/common.sh"
if [[ -f "$UTILS_COMMON" ]]; then
  # shellcheck disable=SC1090
  source "$UTILS_COMMON"
fi

# Create a cross builddir under bin/build-win
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_WIN="$ROOT_DIR/bin/build-win"
CROSS_FILE="$ROOT_DIR/bin/export/cross-windows.ini"

mkdir -p "$BUILD_WIN"

# Setup cross build (source project lives in $ROOT_DIR/bin)
if command -v meson_setup_or_reconfigure >/dev/null 2>&1; then
  meson_setup_or_reconfigure "$BUILD_WIN" "$ROOT_DIR/bin" "release" "$CROSS_FILE"
else
  if [ ! -f "$BUILD_WIN/build.ninja" ]; then
    meson setup --cross-file "$CROSS_FILE" "$BUILD_WIN" "$ROOT_DIR/bin"
  else
    meson setup --reconfigure --cross-file "$CROSS_FILE" "$BUILD_WIN" "$ROOT_DIR/bin"
  fi
fi

# Build only the static library target to avoid running any target executables
meson compile -C "$BUILD_WIN" GGUIcore || {
  echo "Cross compile failed. Check $BUILD_WIN/meson-logs/meson-log.txt" >&2
  exit 1
}
