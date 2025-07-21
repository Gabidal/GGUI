#!/bin/bash

# =============================================================================
# Valgrind Utility Functions for GGUI Analytics
# =============================================================================
# This module provides specialized utilities for Valgrind-based profiling and
# analysis tools including Callgrind, Memcheck, and related functionality.
# =============================================================================

# Source common utilities
UTILS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -z "$COMMON_SOURCED" ]]; then
    source "$UTILS_DIR/common.sh"
    COMMON_SOURCED=true
fi

# -----------------------------------------------------------------------------
# Valgrind Configuration Presets
# -----------------------------------------------------------------------------

##
# Returns Valgrind Callgrind settings for minimal profiling.
#
# Returns:
#   String containing Valgrind options for basic profiling
#
# Usage:
#   settings=$(get_callgrind_minimal_settings)
##
get_callgrind_minimal_settings() {
    echo "--tool=callgrind --dump-instr=yes -s"
}

##
# Returns Valgrind Callgrind settings for comprehensive profiling.
#
# Returns:
#   String containing Valgrind options for full profiling
#
# Usage:
#   settings=$(get_callgrind_full_settings)
##
get_callgrind_full_settings() {
    echo "--tool=callgrind --dump-instr=yes --collect-jumps=yes --simulate-cache=yes --collect-systime=yes --branch-sim=yes"
}

##
# Returns Valgrind Memcheck settings for minimal memory checking.
#
# Arguments:
#   $1 - (Optional) Log file path. Defaults to "valgrind.log"
#
# Returns:
#   String containing Valgrind options for basic memory checking
#
# Usage:
#   settings=$(get_memcheck_minimal_settings)
#   settings=$(get_memcheck_minimal_settings "custom.log")
##
get_memcheck_minimal_settings() {
    local log_file="${1:-valgrind.log}"
    echo "--leak-check=no --tool=memcheck --track-origins=yes --log-file=$log_file"
}

##
# Returns Valgrind Memcheck settings for comprehensive memory checking.
#
# Arguments:
#   $1 - (Optional) Log file path. Defaults to "valgrind.log"
#
# Returns:
#   String containing Valgrind options for full memory checking
#
# Usage:
#   settings=$(get_memcheck_full_settings)
#   settings=$(get_memcheck_full_settings "custom.log")
##
get_memcheck_full_settings() {
    local log_file="${1:-valgrind.log}"
    echo "--leak-check=full --track-origins=yes --show-leak-kinds=all --tool=memcheck --log-file=$log_file"
}

# -----------------------------------------------------------------------------
# Callgrind Operations
# -----------------------------------------------------------------------------

##
# Runs a program under Valgrind Callgrind with specified settings.
#
# Arguments:
#   $1 - Executable path
#   $2 - Profiling mode ("minimal" or "full")
#   $3 - Output file path
#   $4+ - Additional arguments to pass to the executable
#
# Usage:
#   run_callgrind_profile "/path/to/exe" "full" "callgrind.out" "--enableDRM"
##
run_callgrind_profile() {
    local executable="$1"
    local mode="$2"
    local output_file="$3"
    shift 3
    local exe_args="$@"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    local valgrind_settings
    case "$mode" in
        "full")
            valgrind_settings=$(get_callgrind_full_settings)
            log_info "Running Callgrind with full profiling settings"
            ;;
        "minimal"|*)
            valgrind_settings=$(get_callgrind_minimal_settings)
            log_info "Running Callgrind with minimal profiling settings"
            ;;
    esac
    
    log_info "Profiling: $executable with args: $exe_args"
    log_info "Output file: $output_file"
    
    # Validate valgrind is available
    validate_tools "valgrind"
    
    # Run the profiling
    valgrind $valgrind_settings --callgrind-out-file="$output_file" "$executable" $exe_args \
        || handle_error "Valgrind Callgrind profiling failed."
    
    log_info "Profiling completed. Output saved to: $output_file"
}

##
# Runs a program under Valgrind Callgrind with a timeout.
#
# Arguments:
#   $1 - Timeout in seconds
#   $2 - Executable path
#   $3 - Profiling mode ("minimal" or "full")
#   $4 - Output file path
#   $5+ - Additional arguments to pass to the executable
#
# Usage:
#   run_callgrind_timed 30 "/path/to/exe" "full" "callgrind.out"
##
run_callgrind_timed() {
    local timeout_seconds="$1"
    local executable="$2"
    local mode="$3"
    local output_file="$4"
    shift 4
    local exe_args="$@"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    local valgrind_settings
    case "$mode" in
        "full")
            valgrind_settings=$(get_callgrind_full_settings)
            ;;
        "minimal"|*)
            valgrind_settings=$(get_callgrind_minimal_settings)
            ;;
    esac
    
    log_info "Running timed Callgrind profile for ${timeout_seconds}s"
    log_info "Executable: $executable"
    log_info "Mode: $mode"
    log_info "Output: $output_file"
    
    # Validate required tools
    validate_tools "valgrind" "timeout"
    
    # Run with timeout
    timeout "$timeout_seconds" valgrind $valgrind_settings --callgrind-out-file="$output_file" "$executable" $exe_args
    
    local exit_code=$?
    if [ $exit_code -eq 124 ]; then
        log_info "Profiling completed (timeout reached after ${timeout_seconds}s)"
    elif [ $exit_code -ne 0 ]; then
        handle_error "Valgrind Callgrind profiling failed with exit code $exit_code"
    else
        log_info "Profiling completed successfully"
    fi
}

##
# Extracts instruction count from a Callgrind output file.
#
# Arguments:
#   $1 - Callgrind output file path
#
# Returns:
#   Instruction count as a number
#
# Usage:
#   count=$(extract_instruction_count "callgrind.out")
##
extract_instruction_count() {
    local callgrind_file="$1"
    
    if [[ ! -f "$callgrind_file" ]]; then
        handle_error "Callgrind file '$callgrind_file' not found."
    fi
    
    validate_tools "callgrind_annotate"
    
    # Extract instruction count using callgrind_annotate
    local result
    result=$(callgrind_annotate "$callgrind_file" | awk '/^Ir/ {getline; getline; gsub(/,/, "", $1); print $1; exit}' | tail -n1 | tr -d '[:space:]')
    
    if [ -z "$result" ]; then
        handle_error "Failed to extract instruction count from '$callgrind_file'"
    fi
    
    echo "$result"
}

# -----------------------------------------------------------------------------
# Memcheck Operations
# -----------------------------------------------------------------------------

##
# Runs a program under Valgrind Memcheck for memory analysis.
#
# Arguments:
#   $1 - Executable path
#   $2 - Analysis mode ("minimal" or "full")
#   $3 - (Optional) Log file path. Defaults to "valgrind.log"
#   $4+ - Additional arguments to pass to the executable
#
# Usage:
#   run_memcheck_analysis "/path/to/exe" "full" "memory.log"
##
run_memcheck_analysis() {
    local executable="$1"
    local mode="$2"
    local log_file="${3:-valgrind.log}"
    shift 3
    local exe_args="$@"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    local valgrind_settings
    case "$mode" in
        "full")
            valgrind_settings=$(get_memcheck_full_settings "$log_file")
            log_info "Running Memcheck with full memory analysis"
            ;;
        "minimal"|*)
            valgrind_settings=$(get_memcheck_minimal_settings "$log_file")
            log_info "Running Memcheck with minimal memory analysis"
            ;;
    esac
    
    log_info "Analyzing: $executable"
    log_info "Log file: $log_file"
    
    validate_tools "valgrind"
    
    # Run memory analysis
    valgrind $valgrind_settings "$executable" $exe_args
    
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        log_info "Memory analysis completed successfully. Results logged to: $log_file"
    else
        handle_error "Valgrind Memcheck analysis failed with exit code $exit_code"
    fi
}

# -----------------------------------------------------------------------------
# Profile Management
# -----------------------------------------------------------------------------

##
# Opens a Callgrind profile in KCachegrind for visual analysis.
#
# Arguments:
#   $1 - Callgrind output file path
#
# Usage:
#   open_profile_in_kcachegrind "callgrind.out"
##
open_profile_in_kcachegrind() {
    local profile_file="$1"
    
    if [[ ! -f "$profile_file" ]]; then
        handle_error "Profile file '$profile_file' not found."
    fi
    
    validate_tools "kcachegrind"
    
    log_info "Opening profile in KCachegrind: $profile_file"
    kcachegrind "$profile_file" || handle_error "Failed to open KCachegrind."
}

##
# Manages profile file cleanup with user confirmation.
# Offers to save the profile with a timestamp or delete it.
#
# Arguments:
#   $1 - Profile file path
#   $2 - (Optional) Description for user prompt
#
# Usage:
#   manage_profile_cleanup "callgrind.out" "profiling results"
##
manage_profile_cleanup() {
    local profile_file="$1"
    local description="${2:-profile}"
    
    if [[ ! -f "$profile_file" ]]; then
        log_warning "Profile file '$profile_file' not found. Nothing to clean up."
        return 0
    fi
    
    if prompt_yes_no "Do you want to preserve the current $description?" "n"; then
        local backup_name
        backup_name=$(generate_backup_name "$profile_file")
        mv "$profile_file" "$backup_name"
        log_info "$description saved as '$backup_name'."
    else
        rm -f "$profile_file"
        log_info "$description deleted."
    fi
}

# -----------------------------------------------------------------------------
# Utility Functions
# -----------------------------------------------------------------------------

##
# Validates that Valgrind tools are properly installed and accessible.
#
# Usage:
#   validate_valgrind_installation
##
validate_valgrind_installation() {
    validate_tools "valgrind"
    
    # Check if Valgrind can run basic commands
    if ! valgrind --version >/dev/null 2>&1; then
        handle_error "Valgrind is installed but not working properly."
    fi
    
    log_info "Valgrind installation validated successfully."
}
