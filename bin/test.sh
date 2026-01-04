#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Test Runner
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

BUILD_TYPE=${1:-}

meson_setup_or_reconfigure "$BUILD_TYPE"

# Run tests (verbose + print error logs). Extra args are forwarded to meson test.
meson test -C "$(get_build_dir_for_type $BUILD_TYPE)" -v --print-errorlogs "$@"
