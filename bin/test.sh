#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Test Runner
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

# Check if given arguments is one or more
if [[ $# -ge 1 && $# -lt 3 ]]; then   # This means simple mode is active
    BUILD_TYPE=$1
    shift  # Remove first arg

    meson_setup_or_reconfigure "$BUILD_TYPE"

    # Run tests (verbose + print error logs). Extra args are forwarded to meson test.
    meson test -C "$(get_build_dir_for_type $BUILD_TYPE)" -v --print-errorlogs "$@"
elif [[ $# -ge 3 ]]; then
    BUILD_TYPE=$1
    OS=$2
    ARCHITECTURE=$3
    shift 3  # Remove first 3 args, leaving only extras

    # Detect host platform
    HOST_OS="$(get_host_os)"
    HOST_ARCH="$(get_host_arch)"

    # Check if this is native OR if we're already inside Docker container
    # (to avoid recursive Docker calls)
    if [[ "$OS" == "$HOST_OS" && "$ARCHITECTURE" == "$HOST_ARCH" ]] || [[ -f /.dockerenv ]]; then
        # Native or inside Docker: configure Meson and run tests directly
        meson_setup_or_reconfigure_arch "$BUILD_TYPE" "$OS" "$ARCHITECTURE"
        meson test -C "$(get_build_dir_for_arch $BUILD_TYPE $OS $ARCHITECTURE)" -v --print-errorlogs "$@"
    else
        # Non-native: run inside Docker
        IMAGE="ggui-${OS}_${ARCHITECTURE}:latest"
        if ! docker inspect "$IMAGE" >/dev/null 2>&1; then
            echo "Docker image $IMAGE not found. Please build it first."
            exit 1
        fi

        echo "Running tests inside Docker for $OS/$ARCHITECTURE..."
        docker run --rm \
            -v "$PWD:/workspace" \
            "$IMAGE" \
            bash -c "./bin/test.sh $BUILD_TYPE $OS $ARCHITECTURE $@"
    fi
else
    echo "Usage:"
    echo "  Simple mode:   $0 BUILD_TYPE"
    echo "  Advanced mode: $0 BUILD_TYPE OS ARCHITECTURE"
    exit 1
fi
