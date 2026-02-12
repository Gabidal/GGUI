#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Test Runner
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

compile_meson_test_commands() {
    # $1, $2, $3 = build_type, os, arch
    # $4+ = extra test args (optional test name filters)
    local build_type="$1"
    local os="$2"
    local arch="$3"
    shift 3
    local extra_args="$*"
    
    echo "source bin/analytics/utils/common.sh && " \
        "meson_setup_or_reconfigure_arch $build_type $os $arch && " \
        "meson test -C \$(get_build_dir_for_arch $build_type $os $arch) -v --print-errorlogs $extra_args"
}

# Check if given arguments is one or more
if [[ $# -ge 1 && $# -lt 3 ]]; then   # This means simple mode is active
    BUILD_TYPE=$1
    shift  # Remove first arg

    meson_setup_or_reconfigure "$BUILD_TYPE"

    # Run tests (verbose + print error logs). Extra args are forwarded to meson test.
    meson test -C "$(get_build_dir_for_type $BUILD_TYPE)" -v --print-errorlogs "$@"
elif [[ $# -ge 3 ]]; then    # Outside Docker, will call docker and run test.sh inside it.
    BUILD_TYPE=$1
    OS=$2
    ARCHITECTURE=$3
    shift 3  # Remove first 3 args, leaving only extras

    # (to avoid recursive Docker calls)
    if [[ -f /.dockerenv ]]; then
        # Inside Docker: configure Meson and run tests directly
        bash -c "$(compile_meson_test_commands "$BUILD_TYPE" "$OS" "$ARCHITECTURE" "$@")"
    else
        run_docker_build "$OS" "$ARCHITECTURE" "$(compile_meson_test_commands "$BUILD_TYPE" "$OS" "$ARCHITECTURE" "$@")"
    fi
else
    echo "Usage:"
    echo "  Simple mode:   $0 BUILD_TYPE"
    echo "  Advanced mode: $0 BUILD_TYPE OS ARCHITECTURE"
    exit 1
fi
