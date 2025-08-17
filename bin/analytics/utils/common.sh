#!/usr/bin/env bash

# =============================================================================
# Common Utility Functions for GGUI Analytics Scripts
# =============================================================================
# This module provides common functionality used across analytics scripts:
# - Directory management and navigation
# - Error handling and logging
# - Project building
# - User interaction utilities
# =============================================================================

# -----------------------------------------------------------------------------
# Global Configuration
# -----------------------------------------------------------------------------
if [[ -z "$PROJECT_NAME" ]]; then
    readonly PROJECT_NAME="GGUI"
fi
if [[ -z "$BIN_DIR_NAME" ]]; then
    readonly BIN_DIR_NAME="bin"
fi

# Mark that common utilities have been sourced
COMMON_SOURCED=true

# -----------------------------------------------------------------------------
# Error Handling
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
# Directory Management
# -----------------------------------------------------------------------------

##
# Checks if the script is run from the project root directory or a subdirectory
# and navigates to the appropriate bin directory. This function ensures all
# analytics scripts run from the correct working directory.
#
# The function:
# 1. Finds the project root using git
# 2. If in project root, changes to bin directory
# 3. If not in bin directory, navigates to bin directory
#
# Exits with error code 1 if:
# - Project root cannot be determined
# - Navigation to bin directory fails
#
# Usage:
#   ensure_bin_directory
##
ensure_bin_directory() {
    local current_dir=$(pwd)
    
    # Find the project root directory by looking for the .git directory
    local project_root
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)
    
    # If git is not found or the project root is not determined
    if [ -z "$project_root" ]; then
        handle_error "Unable to determine the project root directory. Ensure you're in the $PROJECT_NAME project."
    fi
    
    # If we're in the project root, change to the 'bin' directory
    if [ "$(basename "$project_root")" == "$PROJECT_NAME" ] && [ "$current_dir" == "$project_root" ]; then
        log_info "Project root directory detected. Changing to the '$BIN_DIR_NAME' directory."
        cd "$project_root/$BIN_DIR_NAME" || handle_error "Failed to navigate to $BIN_DIR_NAME directory"
    # Otherwise, navigate to the 'bin' directory from anywhere in the project
    elif [[ "$current_dir" != *"$project_root/$BIN_DIR_NAME"* ]]; then
        log_info "Navigating to the '$BIN_DIR_NAME' directory within the project."
        cd "$project_root/$BIN_DIR_NAME" || handle_error "Failed to navigate to $BIN_DIR_NAME directory"
    fi
}

##
# Gets the current working directory after ensuring we're in the bin directory.
#
# Returns:
#   Absolute path to the current bin directory
#
# Usage:
#   current_dir=$(get_bin_directory)
##
get_bin_directory() {
    ensure_bin_directory
    pwd
}

# -----------------------------------------------------------------------------
# Build Management
# -----------------------------------------------------------------------------

##
# Verifies that the build script exists and executes it.
# This function handles the common pattern of building the GGUI project
# before running analytics tools.
#
# Arguments:
#   $1 - (Optional) Path to build script. Defaults to "./build.sh"
#
# Usage:
#   build_project
#   build_project "/path/to/custom/build.sh"
##
build_project() {
    local build_script="${1:-$(pwd)/build.sh}"
    
    if [[ ! -f "$build_script" ]]; then
        handle_error "Build script '$build_script' not found."
    fi
    
    log_info "Building the project using: $build_script"
    "$build_script" || handle_error "Build process failed."
    log_info "Build completed successfully."
}

##
# Verifies that the GGUI executable exists, building it if necessary.
#
# Arguments:
#   $1 - (Optional) Path to executable. Defaults to auto-detected path
#
# Returns:
#   Absolute path to the verified executable
#
# Usage:
#   executable=$(ensure_executable)
#   executable=$(ensure_executable "/custom/path/to/GGUI")
##
ensure_executable() {
    local executable_path="$1"
    
    # Auto-detect executable path if not provided
    if [[ -z "$executable_path" ]]; then
        local current_dir=$(pwd)
        # Try common build directory locations
        if [[ -f "$current_dir/build/GGUI" ]]; then
            executable_path="$current_dir/build/GGUI"
        elif [[ -f "$current_dir/build/GGUI" ]]; then
            executable_path="$current_dir/build/GGUI"
        else
            executable_path="$current_dir/build/GGUI"  # Default fallback
        fi
    fi
    
    if [[ ! -f "$executable_path" ]]; then
        log_warning "Executable '$executable_path' not found. Attempting to build..."
        build_project
        
        # Verify the executable was created
        if [[ ! -f "$executable_path" ]]; then
            handle_error "Executable '$executable_path' still not found after build."
        fi
    fi
    
    log_info "Using executable: $executable_path" >&2
    echo "$executable_path"
}

# -----------------------------------------------------------------------------
# User Interaction
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------------

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

# -----------------------------------------------------------------------------
# File Management
# -----------------------------------------------------------------------------

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
