#!/usr/bin/env bash
set -euo pipefail

# =============================================================================
# GGUI Project Export Script
# =============================================================================
# Builds GGUI static libraries for multiple platforms using Docker.
# Native build runs directly, other platforms use Docker containers.
#
# Platform Matrix:
#   OS:           linux, windows
#   Architecture: x86, x86_64, arm32, arm64
#
# Output naming: libggui_{os}_{arch}.{a|lib}
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

EXPORT_DIR="$(go_to_project_root)/bin/export"

# =============================================================================
# Platform Configuration
# =============================================================================
SUPPORTED_OS=("linux" "windows")
SUPPORTED_ARCH=("x86" "x86_64" "arm32" "arm64")

##
# Rename build artifact to standardized name
#
# Arguments:
#   $1 - OS name  
#   $2 - Architecture name
##
rename_artifact() {
    local os="$1"
    local arch="$2"
    local ext
    ext="$(get_lib_extension "$os")"
    
    local src_name="libggui${ext}"
    local dst_name="libggui_${os}_${arch}${ext}"
    
    if [[ -f "$EXPORT_DIR/$src_name" ]]; then
        mv "$EXPORT_DIR/$src_name" "$EXPORT_DIR/$dst_name"
        log_info "Created: $dst_name"
        return 0
    else
        log_warning "Artifact not found: $src_name"
        return 1
    fi
}

##
# Compiles the commands given to the docker run
#
# Arguments:
#   $1 - OS name
#   $2 - Architecture name
##
compile_command_for_docker_run() {
    #"source bin/analytics/utils/common.sh && meson_setup_or_reconfigure_arch release linux arm64 bin/export/cross-linux-arm64.ini && meson_compile_target_arch release linux arm64 build_native_archive"
    echo "source bin/analytics/utils/common.sh && " \
        "meson_setup_or_reconfigure_arch release $1 $2 bin/export/cross-$1-$2.ini && " \
        "meson_compile_target_arch release $1 $2 build_native_archive"
}

##
# Build a single platform
#
# Arguments:
#   $1 - OS name
#   $2 - Architecture name
##
build_binaries() {
    local os="$1"
    local arch="$2"
    
    if [[ -z "$os" || -z "$arch" ]]; then
        handle_error "Usage: build_binaries <os> <arch>"
    fi

    if ! build_docker_image "$os" "$arch"; then
        return 1
    fi
    
    if ! run_docker_build "$os" "$arch" $(compile_command_for_docker_run $os $arch); then
        return 1
    fi
    
    rename_artifact "$os" "$arch"
    return $?
}

build_all() {
    # Track results
    local successful=()
    local failed=()
    
    # Build all platforms
    for os in "${SUPPORTED_OS[@]}"; do
        for arch in "${SUPPORTED_ARCH[@]}"; do

            local type=${os}_${arch}

            # Check if Dockerfile exists for this combination
            local dockerfile_path="$EXPORT_DIR/Dockerfile.${type}"
            if [[ ! -f "$dockerfile_path" ]]; then
                log_warning "No Dockerfile for ${type} - skipping"
                continue
            fi

            if build_binaries "$os" "$arch"; then
                successful+=("${type}")
            else
                failed+=("${type}")
            fi
        done
    done
    
    # Summary
    if [[ ${#successful[@]} -gt 0 ]]; then
        echo "Successful builds:"
        for artifact in "${successful[@]}"; do
            echo "  - $artifact"
        done
    fi
    
    if [[ ${#failed[@]} -gt 0 ]]; then
        echo "Failed builds:"
        for platform in "${failed[@]}"; do
            echo "  - $platform"
        done
    fi
    
    # Return error if any builds failed
    if [[ ${#failed[@]} -gt 0 ]]; then
        return 1
    fi
    return 0
}

#---------------------------------------------------------------------#

# Check if no arguments given, if so then call build_all
if [[ $# -eq 0 ]]; then
    build_all
else
    #   $1 - OS name
    #   $2 - Architecture name
    build_binaries "$@"

    header_os="$1"
    header_arch="$2"
fi

# Build headers
meson_setup_or_reconfigure release
meson_compile_target release generate_header
meson_compile_target release generate_dev_header

echo ""
echo "Artifacts location: $EXPORT_DIR/"
