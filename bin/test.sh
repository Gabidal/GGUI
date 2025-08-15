#!/usr/bin/env bash
set -euo pipefail

# SPDX-License-Identifier: MIT
# Run the test suite via Meson (build + export artifacts + tests handled by meson.build)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
BUILD_DIR="${ROOT_DIR}/bin/build"

pushd "${ROOT_DIR}/bin" >/dev/null

# Configure build directory if missing, otherwise reconfigure to pick up changes
if [ ! -d "${BUILD_DIR}" ]; then
    meson setup "${BUILD_DIR}"
else
    meson setup --reconfigure "${BUILD_DIR}"
fi

# Run tests (verbose + print error logs). Extra args are forwarded to meson test.
meson test -C "${BUILD_DIR}" -v --print-errorlogs "$@"

popd >/dev/null
