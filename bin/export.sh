#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Project Export Script
# =============================================================================
# This script initializes the GGUI project with release options enabled.
# Builds it for native and other platforms and moves the builded libraries into ./bin/export/*
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

EXPORT_DIR="$(go_to_project_root)/bin/export"

meson_setup_or_reconfigure "release"

echo "running tests..."
if ! meson test -C "$(get_build_dir_for_type "release")" -v --print-errorlogs; then
	err "meson tests failed; aborting export"
	exit 1
fi

# Export native artifacts (header + Linux static lib) via Meson run target from release build
meson_compile_target "release" "build_native_archive"

# 5) Export Windows cross-compiled lib, if MinGW is available (use release build)
# Detect common MinGW cross-compilers or allow user to provide CROSS_COMPILE_PREFIX env var
MINGW_PREFIX="${CROSS_COMPILE_PREFIX:-}"
if [ -z "${MINGW_PREFIX}" ]; then
	for pfx in x86_64-w64-mingw32- x86_64-w64-mingw32-gnu- i686-w64-mingw32-; do
		if command -v "${pfx}c++" >/dev/null 2>&1; then
			MINGW_PREFIX="${pfx%c++}"
			break
		fi
	done
fi

if [ -n "${MINGW_PREFIX}" ]; then
	echo "exporting Windows artifacts (cross-compile) using prefix ${MINGW_PREFIX} from release build"
	meson_compile_target "release" "export-win"
else
	echo "MinGW cross-compiler not found; skipping Windows export. Install: sudo apt install c++-mingw-w64-x86-64-posix"
fi

echo "Exported artifacts are available in ${EXPORT_DIR}/"
