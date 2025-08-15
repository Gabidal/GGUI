#!/usr/bin/env bash
set -euo pipefail

# Cross-compile Linux artifacts from a Windows host using a Linux cross toolchain.
# Requires a working bash (e.g., Git Bash) and a cross toolchain on PATH,
# such as x86_64-linux-gnu-* from MSYS2 or a custom toolchain.

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_LINUX="$ROOT_DIR/bin/build-linux"
CROSS_FILE="$ROOT_DIR/bin/export/cross-linux.ini"

mkdir -p "$BUILD_LINUX"

# Setup cross build targeting Linux if not already configured
if [ ! -f "$BUILD_LINUX/build.ninja" ]; then
  meson setup --cross-file "$CROSS_FILE" "$BUILD_LINUX" "$ROOT_DIR/bin"
fi

# Build only the core library to avoid running any target executables
meson compile -C "$BUILD_LINUX" GGUIcore || {
  echo "Cross compile (Linux) failed. Check $BUILD_LINUX/meson-logs/meson-log.txt" >&2
  exit 1
}

# Export the Linux static library into bin/export
LINUX_LIB_SRC="$(find "$BUILD_LINUX" -type f -name 'libGGUIcore.a' -o -name 'libGGUIUnix.a' | head -n1)"
if [ -z "$LINUX_LIB_SRC" ]; then
  # If Meson created only objects, manually archive
  # Try to create an archive from objects as a fallback
  OBJ_LIST=$(find "$BUILD_LINUX" -type f -name '*.o' | tr '\n' ' ')
  if [ -n "$OBJ_LIST" ]; then
  ar rcs "$ROOT_DIR/bin/export/libGGUIUnix.a" $OBJ_LIST
    LINUX_LIB_SRC="$ROOT_DIR/bin/export/libGGUIUnix.a"
  fi
fi

if [ -z "$LINUX_LIB_SRC" ]; then
  echo "Could not find or create a Linux static library in $BUILD_LINUX" >&2
  exit 1
fi

OUT_LIB="$ROOT_DIR/bin/export/libGGUIUnix.a"
cp "$LINUX_LIB_SRC" "$OUT_LIB"
echo "Exported Linux library to $OUT_LIB"
