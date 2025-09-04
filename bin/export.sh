#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Export Orchestrator (Linux)
# - Ensure native build exists and tests pass (bin/build)
# - Prepare a release build (bin/build-release)
# - Export native artifacts (header + platform lib) via Meson run target
# - Attempt Windows cross export via export-win if bash and script available
# =============================================================================

SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"      # /root/GGUI/bin (meson.build lives here)
BUILD_DIR="${SOURCE_DIR}/build"                                 # /root/GGUI/bin/build (used for tests)
BUILD_DIR_RELEASE="${SOURCE_DIR}/build-release"                 # dedicated release build for exported artifacts
EXPORT_DIR="${SOURCE_DIR}/export"                               # /root/GGUI/bin/export

log() { echo "[export] $*"; }
err() { echo "[export] ERROR: $*" >&2; }

trap 'log "failed"' ERR

# 1) Ensure native build directory exists and is up to date
log "configuring native build at ${BUILD_DIR} (source: ${SOURCE_DIR})"
if [ ! -d "${BUILD_DIR}" ]; then
	meson setup "${BUILD_DIR}" "${SOURCE_DIR}"
else
	meson setup --reconfigure "${BUILD_DIR}" "${SOURCE_DIR}"
fi

# 2) Run Meson tests (will also build dependencies like header/lib if needed)
log "running tests"
if ! meson test -C "${BUILD_DIR}" -v --print-errorlogs; then
	err "meson tests failed; aborting export"
	exit 1
fi

# 3) Prepare a dedicated release build directory so exported artifacts use -Dbuildtype=release
log "configuring release build at ${BUILD_DIR_RELEASE} (buildtype=release)"
if [ ! -d "${BUILD_DIR_RELEASE}" ]; then
	meson setup "${BUILD_DIR_RELEASE}" "${SOURCE_DIR}" -Dbuildtype=release
else
	# Try to set buildtype via meson configure; fall back to reconfigure
	if ! meson configure "${BUILD_DIR_RELEASE}" -Dbuildtype=release >/dev/null 2>&1; then
		meson setup --reconfigure "${BUILD_DIR_RELEASE}" "${SOURCE_DIR}" -Dbuildtype=release
	fi
fi

# 4) Export native artifacts (header + Linux static lib) via Meson run target from release build
log "exporting native artifacts from release build"
meson compile -C "${BUILD_DIR_RELEASE}"

# 5) Export Windows cross-compiled lib, if MinGW is available (use release build)
# Detect common MinGW cross-compilers or allow user to provide CROSS_COMPILE_PREFIX env var
MINGW_PREFIX="${CROSS_COMPILE_PREFIX:-}"
if [ -z "${MINGW_PREFIX}" ]; then
	for pfx in x86_64-w64-mingw32- x86_64-w64-mingw32-gnu- i686-w64-mingw32-; do
		if command -v "${pfx}g++" >/dev/null 2>&1; then
			MINGW_PREFIX="${pfx%g++}"
			break
		fi
	done
fi

if [ -n "${MINGW_PREFIX}" ]; then
	log "exporting Windows artifacts (cross-compile) using prefix ${MINGW_PREFIX} from release build"
	meson compile -C "${BUILD_DIR_RELEASE}" export-win || log "Windows export failed"
else
	log "MinGW cross-compiler not found; skipping Windows export. Install: sudo apt install g++-mingw-w64-x86-64-posix"
fi

echo "Exported artifacts are available in ${EXPORT_DIR}/"
