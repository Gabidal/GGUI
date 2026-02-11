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
# Returns the library file extension for a given OS.
#
# Arguments:
#   $1 - OS name (linux, windows)
#
# Returns:
#   .a for linux, .lib for windows
##
get_lib_extension() {
    local os="$1"
    case "$os" in
        linux)   echo ".a" ;;
        windows) echo ".lib" ;;
        *)       echo ".a" ;;
    esac
}

##
# Remap platform name to Docker platform specification
#
# Maps OS and architecture to Docker's platform format for cross-compilation.
# Returns empty string for native builds or unsupported combinations.
#
# Docker Platform Format: <os>/<arch>[/<variant>]
#
# Arguments:
#   $1 - OS name (linux, windows)
#   $2 - Architecture name (x86, x86_64, arm32, arm64)
#
# Returns:
#   Docker platform string (e.g., "linux/arm64") or empty for native/unsupported
##
remap_platform_name() {
    local os="$1"
    local arch="$2"
    local platform=""
    
    case "${os}_${arch}" in
        # Linux platforms
        linux_x86_64)  platform="linux/amd64" ;;
        linux_x86)     platform="linux/386" ;;
        linux_arm64)   platform="linux/arm64" ;;
        linux_arm32)   platform="linux/arm/v7" ;;
        
        # Windows platforms (requires Windows containers or MinGW cross-compilation)
        windows_x86_64) platform="linux/amd64" ;;  # Cross-compile with MinGW on Linux
        windows_x86)    platform="linux/386" ;;    # Cross-compile with MinGW on Linux
        windows_arm64)  platform="linux/arm64" ;;  # Cross-compile with MinGW on ARM64 Linux
        
        # Unsupported or native - return empty
        *)             platform="" ;;
    esac
    
    echo "$platform"
}

##
# Build Docker image for a platform
#
# Arguments:
#   $1 - OS name
#   $2 - Architecture name
##
build_docker_image() {
    local os="$1"
    local arch="$2"
    local image_name="ggui-${os}_${arch}:latest"
    local dockerfile_path="$EXPORT_DIR/Dockerfile.${os}_${arch}"
    local docker_platform="$(remap_platform_name "$os" "$arch")"
    
    if [[ ! -f "$dockerfile_path" ]]; then
        handle_error "Dockerfile not found: $dockerfile_path"
    fi
    
    log_info "Building Docker image: $image_name"
    
    local build_args=("build" "-f" "$dockerfile_path" "-t" "$image_name")
    
    if [[ -n "$docker_platform" ]]; then
        build_args+=("--platform" "$docker_platform")
    fi
    
    build_args+=("$(go_to_project_root)")
    
    if ! docker "${build_args[@]}"; then
        log_warning "Failed to build Docker image for ${os}_${arch}"
        return 1
    fi
    
    log_info "Successfully built: $image_name"
    return 0
}

##
# Run Docker container to build for a platform
#
# Arguments:
#   $1 - OS name
#   $2 - Architecture name
##
run_docker_build() {
    local os="$1"
    local arch="$2"
    local image_name="ggui-${os}_${arch}:latest"
    local docker_platform
    docker_platform="$(remap_platform_name "$os" "$arch")"
    local project_root
    project_root="$(go_to_project_root)"
    
    log_info "Building ${os}_${arch} via Docker..."
    
    local run_args=("run" "--rm" "-v" "${project_root}:/workspace")
    
    if [[ -n "$docker_platform" ]]; then
        run_args+=("--platform" "$docker_platform")
    fi
    
    run_args+=("$image_name")
    
    if ! docker "${run_args[@]}"; then
        log_warning "Docker build failed for ${os}_${arch}"
        return 1
    fi
    
    return 0
}

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
    
    if ! run_docker_build "$os" "$arch"; then
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
    
    echo ""
    echo "Artifacts location: $EXPORT_DIR/"
    
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
fi
