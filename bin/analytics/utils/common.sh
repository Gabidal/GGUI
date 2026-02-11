#!/usr/bin/env bash

# =============================================================================
# Common Utility Functions for GGUI Analytics Scripts
# =============================================================================

# Mark that common utilities have been sourced
COMMON_SOURCED=true

##
# Handles errors and exits gracefully with an error message.
# Outputs error message to stderr and exits with code 1.
#
# Arguments:
#   $1 - Error message to display
#
# Usage:
#   handle_error "Build process failed"
##
handle_error() {
    local error_message="$1"
    echo "Error: $error_message" >&2
    exit 1
}

##
# Logs informational messages with a consistent format.
#
# Arguments:
#   $1 - Message to log
#
# Usage:
#   log_info "Starting build process..."
##
log_info() {
    local message="$1"
    echo "[INFO] $message"
}

##
# Logs warning messages with a consistent format.
#
# Arguments:
#   $1 - Warning message to log
#
# Usage:
#   log_warning "Build directory already exists"
##
log_warning() {
    local message="$1"
    echo "[WARNING] $message" >&2
}

##
# Returns project root directory. And changes directory into there.
#
# Usage:
#   go_to_project_root
##
go_to_project_root() {
    local current_dir=$(pwd)
    local project_root=""
    
    # Try git first (works in most development environments)
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)
    
    # Fallback for Docker/CI environments where git might not work
    if [ -z "$project_root" ]; then
        # Check if we're in /workspace (Docker standard mount point)
        if [ -d "/workspace/bin" ] && [ -f "/workspace/bin/meson.build" ]; then
            project_root="/workspace"
        # Check current directory (look for bin/meson.build as project marker)
        elif [ -d "$current_dir/bin" ] && [ -f "$current_dir/bin/meson.build" ]; then
            project_root="$current_dir"
        # Search upward for project markers
        else
            local search_dir="$current_dir"
            while [ "$search_dir" != "/" ]; do
                if [ -d "$search_dir/bin" ] && [ -f "$search_dir/bin/meson.build" ]; then
                    project_root="$search_dir"
                    break
                fi
                search_dir=$(dirname "$search_dir")
            done
        fi
    fi
    
    # If still not found, error out
    if [ -z "$project_root" ]; then
        handle_error "Unable to determine the project root directory."
    fi
    
    # Change to project root if not already there
    if [ "$current_dir" != "$project_root" ]; then
        cd "$project_root" || handle_error "Failed to change to project root directory."
    fi

    echo $(pwd)
}

##
# Returns the absolute Meson build directory for a given build type.
#
# Arguments:
#   $1 - build type: "debug" (default) or "release"
#
# Echoes the absolute path to the build directory.
##
get_build_dir_for_type() {
    local build_type="${1:-debug}"
    local base_dir="$(go_to_project_root)/bin"
    local result="$base_dir/build"
    if [[ "$build_type" == "release" ]]; then
        result="$base_dir/build-release"
    elif [[ "$build_type" == "profile" ]]; then
        result="$base_dir/build-profile"
    fi

    echo "$result"
}

##
# Verifies that the build script exists and executes it.
# This function handles the common pattern of building the GGUI project
# before running analytics tools.
#
# Arguments:
#   $1 - Build type to build for
#   $2+ - Target name(s) (defaults to ggui)
#
# Usage:
#   build_project profile
#   build_project release
#   build_project debug
##
build_project() {
    local build_type="$1"
    local build_target="${2:-ggui}"
    local build_script="$(go_to_project_root)/bin/build.sh"
    $build_script $build_type $build_target || handle_error "Build process failed."
}

##
# Verifies that the ggui executable exists, building it if necessary.
#
# Arguments:
#   $1 - Build type
#
# Returns:
#   Absolute path to the verified executable
#
# Usage:
#   executable=$(ensure_executable profile)
#   executable=$(ensure_executable release)
#   executable=$(ensure_executable debug)
##
ensure_executable() {
    local build_type="$1"
    local build_dir=$(get_build_dir_for_type "$build_type")
    local executable_path="$build_dir/ggui"

    if [[ ! -f "$executable_path" ]]; then
        log_warning "Executable '$executable_path' not found. Attempting to build..."
        build_project $build_type
        
        # Verify the executable was created
        if [[ ! -f "$executable_path" ]]; then
            handle_error "Executable '$executable_path' still not found after build."
        fi
    fi
    
    log_info "Using executable: $executable_path" >&2
    echo "$executable_path"
}

##
# Compiles the Meson build for the requested type using meson compile -C <dir>.
#
# Arguments:
#   $1 - build type: "debug" (default) or "release" or "profile"
#   $2 - target (defaults into build_native_archive)
#
# Usage:
#   meson_compile_target release
##
meson_compile_target() {
    local build_type="${1:-debug}"
    local targets="${2:-build_native_archive}"
    local build_dir="$(get_build_dir_for_type "$build_type")"
    local jobs="${MESON_JOBS:-0}"  # 0 = auto-detect all cores

    log_info "Compiling ${build_type} build at ${build_dir}"
    meson compile -C "${build_dir}" -j "${jobs}" ${targets} || handle_error "Build failed for ${build_type} (${build_dir})"
}

get_host_os() {
    case "$(uname -s)" in
        Linux*)                 echo        "linux" ;;
        Darwin*)                echo        "macos" ;;
        MINGW*|MSYS*|CYGWIN*)   echo        "windows" ;;
        *)                      echo        "unknown" ;;
    esac
}

get_host_arch() {
    case "$(uname -m)" in
        x86_64|amd64)           echo        "x86_64" ;;
        i386|i686)              echo        "x86" ;;
        aarch64|arm64)          echo        "arm64" ;;
        armv7l)                 echo        "arm32" ;;
        *)                      echo        "unknown" ;;
    esac
}

##
# Returns the absolute Meson build directory for a given build type and architecture.
#
# Arguments:
#   $1 - build type: "debug", "release", or "profile"
#   $2 - OS name: e.g., "linux", "windows"
#   $3 - architecture: e.g., "arm32", "arm64", "x86", etc.
#
# Echoes the absolute path to the build directory.
##
get_build_dir_for_arch() {
    local build_type="$1"
    local os_name="$2"
    local architecture="$3"
    local base_dir="$(go_to_project_root)/bin"
    
    echo "$base_dir/build-${build_type}-${os_name}-${architecture}"
}

##
# Ensures a Meson build directory is configured for a specific architecture.
#
# Arguments:
#   $1 - build type (profile, release, debug)
#   $2 - OS name (linux, windows)
#   $3 - architecture name (arm32, arm64, x86, x86_64, etc.)
#   $4 - (Optional) cross-file path for cross-compilation
##
meson_setup_or_reconfigure_arch() {
    local build_type="$1"
    local os_name="$2"
    local arch_name="$3"
    local cross_file="${4:-}"
    local build_dir="$(get_build_dir_for_arch "$build_type" "$os_name" "$arch_name")"
    local meson_build_location="$(go_to_project_root)/bin"
    local meson_build_type="-Dbuildtype=$(meson_remap_build_type "$build_type")"
    local meson_args=("$meson_build_type")
    
    # Add cross-file if provided
    if [[ -n "$cross_file" ]]; then
        meson_args+=("--cross-file" "$cross_file")
    fi

    if [[ -d "$build_dir" ]]; then
        if ! meson setup --reconfigure "$build_dir" "$meson_build_location" "${meson_args[@]}"; then
            rm -rf "$build_dir" # If reconfiguration fails, try from a clean slate.
        fi
    fi
    
    log_info "Configuring Meson build directory: $build_dir (${os_name}/${arch_name})"
    meson setup "$build_dir" "$meson_build_location" "${meson_args[@]}" || handle_error "Meson setup failed for $build_dir"
}

##
# Compiles the Meson build for a specific architecture.
#
# Arguments:
#   $1 - build type: "debug", "release", or "profile"
#   $2 - OS name: e.g., "linux", "windows"
#   $3 - architecture name: e.g., "arm32", "arm64", "x86", "x86_64"
#   $4 - target (defaults to build_native_archive)
#
# Usage:
#   meson_compile_target_arch release linux arm32 build_native_archive
##
meson_compile_target_arch() {
    local build_type="$1"
    local os_name="$2"
    local arch_name="$3"
    local targets="${4:-build_native_archive}"
    local build_dir="$(get_build_dir_for_arch "$build_type" "$os_name" "$arch_name")"
    local jobs="${MESON_JOBS:-0}"  # 0 = auto-detect all cores

    log_info "Compiling ${build_type} build for ${os_name}/${arch_name} at ${build_dir}"
    meson compile -C "${build_dir}" -j "${jobs}" ${targets} || handle_error "Build failed for ${build_dir}"
}

meson_remap_build_type() {
    local build_type="$1"

    if ([[ "$build_type" == "debug" ]] || [[ -z "$build_type" ]]); then
        echo "debug"
    elif [[ "$build_type" == "release" ]]; then
        echo "release"
    elif [[ "$build_type" == "profile" ]]; then
        echo "debugoptimized"
    else
        handle_error "Unknown build type: $build_type"
    fi
}

##
# Ensures a Meson build directory is configured (setup or reconfigure).
#
# Arguments:
#   $1 - build type (profile, release, debug)
##
meson_setup_or_reconfigure() {
    local build_type="$1"
    local build_dir="$(get_build_dir_for_type "$build_type")"
    local meson_build_location="$(go_to_project_root)/bin"
    local meson_build_type="-Dbuildtype=$(meson_remap_build_type "$build_type")"

    if [[ -d "$build_dir" ]]; then
        if ! meson setup --reconfigure "$build_dir" "$meson_build_location" "$meson_build_type"; then
            rm -rf "$build_dir" # If reconfiguration fails, try from a clean slate.
        fi
    fi
    
    log_info "Configuring Meson build directory: $build_dir"
    meson setup "$build_dir" "$meson_build_location" "$meson_build_type" || handle_error "Meson setup failed for $build_dir"
}

##
# Prompts user with a yes/no question and returns the result.
#
# Arguments:
#   $1 - Question to ask the user
#   $2 - (Optional) Default answer (y/n). Defaults to 'n'
#
# Returns:
#   0 for yes, 1 for no
#
# Usage:
#   if prompt_yes_no "Continue with operation?"; then
#       echo "User chose yes"
#   fi
##
prompt_yes_no() {
    local question="$1"
    local default="${2:-n}"
    local prompt_suffix
    
    if [[ "$default" == "y" ]]; then
        prompt_suffix="[Y/n]"
    else
        prompt_suffix="[y/N]"
    fi
    
    echo "$question $prompt_suffix"
    read -r answer
    
    # Use default if no answer provided
    if [[ -z "$answer" ]]; then
        answer="$default"
    fi
    
    [[ "$answer" =~ ^(Y|y)$ ]]
}

##
# Provides a countdown timer with user feedback.
#
# Arguments:
#   $1 - Number of seconds to wait
#   $2 - (Optional) Message to display. Defaults to "Starting in"
#
# Usage:
#   countdown_timer 5 "Benchmark starting in"
##
countdown_timer() {
    local seconds="$1"
    local message="${2:-Starting in}"
    
    for ((i=seconds; i>0; i--)); do
        echo "$message $i seconds..."
        sleep 1
    done
}

##
# Validates that required tools are available on the system.
#
# Arguments:
#   $@ - List of tool names to check
#
# Usage:
#   validate_tools "valgrind" "kcachegrind" "perf"
##
validate_tools() {
    local missing_tools=()
    
    for tool in "$@"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        handle_error "Missing required tools: ${missing_tools[*]}. Please install them before continuing."
    fi
}

##
# Validates that a specific tool exists at a given path.
#
# Arguments:
#   $1 - Tool name (for error messages)
#   $2 - Path to tool
#
# Usage:
#   validate_tool_path "perf" "/usr/local/bin/perf"
##
validate_tool_path() {
    local tool_name="$1"
    local tool_path="$2"
    
    if [[ ! -x "$tool_path" ]]; then
        handle_error "'$tool_path' ($tool_name) is not installed or not executable."
    fi
}

##
# Generates a timestamp-based backup filename.
#
# Arguments:
#   $1 - Base filename
#   $2 - (Optional) File extension. Defaults to "backup"
#
# Returns:
#   Timestamped filename
#
# Usage:
#   backup_name=$(generate_backup_name "callgrind.out")
##
generate_backup_name() {
    local base_filename="$1"
    local extension="${2:-backup}"
    local timestamp=$(date '+%Y-%m-%d_%H-%M-%S')
    
    echo "${base_filename}.${timestamp}.${extension}"
}
