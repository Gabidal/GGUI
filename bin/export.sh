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
# Returns the Docker platform flag for multi-arch builds.
#
# Arguments:
#   $1 - OS name
#   $2 - Architecture name
#
# Returns:
#   Docker platform string or empty if not needed
##
get_docker_platform() {
    local os="$1"
    local arch="$2"
    
    case "${os}_${arch}" in
        linux_arm64)  echo "linux/arm64" ;;
        linux_arm32)  echo "linux/arm/v7" ;;
        *)            echo "" ;;
    esac
}

# =============================================================================
# Helper Functions
# =============================================================================

##
# Detect host OS and architecture
##
detect_host_platform() {
    local os arch
    
    case "$(uname -s)" in
        Linux*)  os="linux" ;;
        Darwin*) os="macos" ;;
        MINGW*|MSYS*|CYGWIN*) os="windows" ;;
        *)       os="unknown" ;;
    esac
    
    case "$(uname -m)" in
        x86_64|amd64)     arch="x86_64" ;;
        i386|i686)        arch="x86" ;;
        aarch64|arm64)    arch="arm64" ;;
        armv7l)           arch="arm32" ;;
        *)                arch="unknown" ;;
    esac
    
    echo "${os}:${arch}"
}

##
# Check if Docker is available
##
check_docker() {
    if ! command -v docker >/dev/null 2>&1; then
        log_warning "Docker is not installed - only native builds will be available"
        return 1
    fi
    
    if ! docker info >/dev/null 2>&1; then
        log_warning "Docker daemon is not running - only native builds will be available"
        return 1
    fi
    
    return 0
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
    local docker_platform
    docker_platform="$(get_docker_platform "$os" "$arch")"
    
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
    docker_platform="$(get_docker_platform "$os" "$arch")"
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
#   $3 - Host OS
#   $4 - Host architecture
##
build_platform() {
    local os="$1"
    local arch="$2"
    local host_os="$3"
    local host_arch="$4"
    
    # Check if this is the native platform (host matches target)
    if [[ "$os" == "$host_os" && "$arch" == "$host_arch" ]]; then
        log_info "Building native: ${os}_${arch}"
        meson_setup_or_reconfigure_arch "release" "$os" "$arch"
        meson_compile_target_arch "release" "$os" "$arch" "build_native_archive"
        rename_artifact "$os" "$arch"
        return $?
    fi
    
    # Non-native build via Docker
    if ! build_docker_image "$os" "$arch"; then
        return 1
    fi
    
    if ! run_docker_build "$os" "$arch"; then
        return 1
    fi
    
    rename_artifact "$os" "$arch"
    return $?
}

# =============================================================================
# Main Export Logic
# =============================================================================

main() {
    local host_platform
    host_platform="$(detect_host_platform)"
    local host_os host_arch
    host_os="$(echo "$host_platform" | cut -d: -f1)"
    host_arch="$(echo "$host_platform" | cut -d: -f2)"
    
    echo "=============================================="
    echo "GGUI Export - Host: ${host_os}/${host_arch}"
    echo "=============================================="
    
    # Check Docker availability for non-native platforms
    local docker_available=false
    if check_docker; then
        docker_available=true
    fi
    
    # Run tests first
    log_info "Running tests..."
    meson_setup_or_reconfigure_arch "release" "$host_os" "$host_arch"
    if ! meson test -C "$(get_build_dir_for_arch "release" "$host_os" "$host_arch")" -v --print-errorlogs; then
        handle_error "Tests failed - aborting export"
    fi
    
    # Track results
    local successful=()
    local failed=()
    
    # Build all platforms
    for os in "${SUPPORTED_OS[@]}"; do
        for arch in "${SUPPORTED_ARCH[@]}"; do
            # Check if Dockerfile exists for this combination
            local dockerfile_path="$EXPORT_DIR/Dockerfile.${os}_${arch}"
            if [[ ! -f "$dockerfile_path" ]]; then
                log_warning "No Dockerfile for ${os}_${arch} - skipping"
                continue
            fi
            
            # Check if this is native or requires Docker
            local is_native=false
            if [[ "$os" == "$host_os" && "$arch" == "$host_arch" ]]; then
                is_native=true
            fi
            
            # Non-native builds require Docker
            if [[ "$is_native" == "false" && "$docker_available" == "false" ]]; then
                log_warning "Skipping ${os}_${arch} - Docker not available"
                failed+=("${os}_${arch}")
                continue
            fi
            
            echo ""
            if [[ "$is_native" == "true" ]]; then
                echo "--- Building: ${os}_${arch} (native) ---"
            else
                echo "--- Building: ${os}_${arch} (docker) ---"
            fi
            
            local ext
            ext="$(get_lib_extension "$os")"
            
            if build_platform "$os" "$arch" "$host_os" "$host_arch"; then
                successful+=("libggui_${os}_${arch}${ext}")
            else
                failed+=("${os}_${arch}")
            fi
        done
    done
    
    # Summary
    echo ""
    echo "=============================================="
    echo "Export Summary"
    echo "=============================================="
    
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

main "$@"
