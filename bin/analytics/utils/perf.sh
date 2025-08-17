#!/usr/bin/env bash

# =============================================================================
# Perf Utility Functions for GGUI Analytics
# =============================================================================
# This module provides specialized utilities for Linux perf-based profiling
# and analysis tools including perf record, perf stat, and pprof integration.
# =============================================================================

# Source common utilities
UTILS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -z "$COMMON_SOURCED" ]]; then
    source "$UTILS_DIR/common.sh"
    COMMON_SOURCED=true
fi

# -----------------------------------------------------------------------------
# Perf Configuration Presets
# -----------------------------------------------------------------------------

##
# Returns perf events for basic performance profiling.
#
# Returns:
#   String containing comma-separated perf events
#
# Usage:
#   events=$(get_perf_basic_events)
##
get_perf_basic_events() {
    echo "branch-misses"
}

##
# Returns perf events for comprehensive performance profiling.
#
# Returns:
#   String containing comma-separated perf events
#
# Usage:
#   events=$(get_perf_comprehensive_events)
##
get_perf_comprehensive_events() {
    echo "branch-misses,cpu-cycles,instructions"
}

##
# Returns perf options for basic recording.
#
# Returns:
#   String containing perf record options
#
# Usage:
#   options=$(get_perf_basic_options)
##
get_perf_basic_options() {
    echo "-g"
}

##
# Returns perf options for comprehensive recording.
#
# Returns:
#   String containing perf record options
#
# Usage:
#   options=$(get_perf_comprehensive_options)
##
get_perf_comprehensive_options() {
    echo "-g --freq=1000"
}

# -----------------------------------------------------------------------------
# Perf Recording Operations
# -----------------------------------------------------------------------------

##
# Records performance data using perf with specified events and options.
#
# Arguments:
#   $1 - Executable path
#   $2 - Profiling mode ("basic", "comprehensive", or "custom")
#   $3 - Output file path
#   $4 - (Optional) Custom events (only used with "custom" mode)
#   $5 - (Optional) Custom options (only used with "custom" mode)
#   $6+ - Additional arguments to pass to the executable
#
# Usage:
#   run_perf_record "/path/to/exe" "comprehensive" "perf.data"
#   run_perf_record "/path/to/exe" "custom" "perf.data" "cpu-cycles" "-g --freq=500"
##
run_perf_record() {
    local executable="$1"
    local mode="$2"
    local output_file="$3"
    local custom_events="$4"
    local custom_options="$5"
    shift 5
    local exe_args="$@"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    local perf_events
    local perf_options
    
    case "$mode" in
        "comprehensive")
            perf_events=$(get_perf_comprehensive_events)
            perf_options=$(get_perf_comprehensive_options)
            log_info "Recording with comprehensive perf settings"
            ;;
        "custom")
            perf_events="${custom_events:-$(get_perf_basic_events)}"
            perf_options="${custom_options:-$(get_perf_basic_options)}"
            log_info "Recording with custom perf settings"
            ;;
        "basic"|*)
            perf_events=$(get_perf_basic_events)
            perf_options=$(get_perf_basic_options)
            log_info "Recording with basic perf settings"
            ;;
    esac
    
    log_info "Events: $perf_events"
    log_info "Options: $perf_options"
    log_info "Executable: $executable"
    log_info "Output: $output_file"
    
    # Validate perf availability
    validate_perf_installation
    
    # Run perf record
    /usr/local/bin/perf record -e "$perf_events" $perf_options -o "$output_file" "$executable" $exe_args \
        || handle_error "Perf recording failed."
    
    log_info "Performance recording completed. Data saved to: $output_file"
}

##
# Records performance data with a timeout.
#
# Arguments:
#   $1 - Timeout in seconds
#   $2 - Executable path
#   $3 - Profiling mode ("basic", "comprehensive", or "custom")
#   $4 - Output file path
#   $5 - (Optional) Custom events (only used with "custom" mode)
#   $6 - (Optional) Custom options (only used with "custom" mode)
#   $7+ - Additional arguments to pass to the executable
#
# Usage:
#   run_perf_record_timed 30 "/path/to/exe" "comprehensive" "perf.data"
##
run_perf_record_timed() {
    local timeout_seconds="$1"
    local executable="$2"
    local mode="$3"
    local output_file="$4"
    local custom_events="$5"
    local custom_options="$6"
    shift 6
    local exe_args="$@"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    local perf_events
    local perf_options
    
    case "$mode" in
        "comprehensive")
            perf_events=$(get_perf_comprehensive_events)
            perf_options=$(get_perf_comprehensive_options)
            ;;
        "custom")
            perf_events="${custom_events:-$(get_perf_basic_events)}"
            perf_options="${custom_options:-$(get_perf_basic_options)}"
            ;;
        "basic"|*)
            perf_events=$(get_perf_basic_events)
            perf_options=$(get_perf_basic_options)
            ;;
    esac
    
    log_info "Running timed perf recording for ${timeout_seconds}s"
    log_info "Events: $perf_events"
    log_info "Mode: $mode"
    
    validate_perf_installation
    validate_tools "timeout"
    
    # Run with timeout
    timeout "$timeout_seconds" /usr/local/bin/perf record -e "$perf_events" $perf_options -o "$output_file" "$executable" $exe_args
    
    local exit_code=$?
    if [ $exit_code -eq 124 ]; then
        log_info "Recording completed (timeout reached after ${timeout_seconds}s)"
    elif [ $exit_code -ne 0 ]; then
        handle_error "Perf recording failed with exit code $exit_code"
    else
        log_info "Recording completed successfully"
    fi
}

# -----------------------------------------------------------------------------
# Perf Analysis Operations
# -----------------------------------------------------------------------------

##
# Launches pprof web interface for analyzing perf data.
#
# Arguments:
#   $1 - Executable path (binary that was profiled)
#   $2 - Perf data file path
#   $3 - (Optional) Port number. Defaults to 8080
#   $4 - (Optional) Browser command. Defaults to system default
#
# Usage:
#   launch_pprof_web "/path/to/exe" "perf.data" 8080 "firefox"
##
launch_pprof_web() {
    local executable="$1"
    local perf_data="$2"
    local port="${3:-8080}"
    local browser="${4:-}"
    
    if [[ ! -f "$executable" ]]; then
        handle_error "Executable '$executable' not found."
    fi
    
    if [[ ! -f "$perf_data" ]]; then
        handle_error "Perf data file '$perf_data' not found."
    fi
    
    validate_pprof_installation
    
    # Set browser if specified
    if [[ -n "$browser" ]]; then
        export BROWSER="$browser"
        log_info "Using browser: $browser"
    fi
    
    log_info "Launching pprof web interface on port $port"
    log_info "Executable: $executable"
    log_info "Data file: $perf_data"
    
    /root/go/bin/pprof -http=":$port" "$executable" "$perf_data" \
        || handle_error "Failed to launch pprof analysis."
}

##
# Generates a performance report using perf report.
#
# Arguments:
#   $1 - Perf data file path
#   $2 - (Optional) Output format ("text", "stdio", "tui"). Defaults to "stdio"
#   $3 - (Optional) Output file path. If not specified, outputs to stdout
#
# Usage:
#   generate_perf_report "perf.data" "text" "report.txt"
##
generate_perf_report() {
    local perf_data="$1"
    local format="${2:-stdio}"
    local output_file="$3"
    
    if [[ ! -f "$perf_data" ]]; then
        handle_error "Perf data file '$perf_data' not found."
    fi
    
    validate_perf_installation
    
    local perf_cmd="/usr/local/bin/perf report -i '$perf_data' --$format"
    
    if [[ -n "$output_file" ]]; then
        perf_cmd="$perf_cmd > '$output_file'"
        log_info "Generating perf report to: $output_file"
    else
        log_info "Generating perf report to stdout"
    fi
    
    eval "$perf_cmd" || handle_error "Failed to generate perf report."
    
    if [[ -n "$output_file" ]]; then
        log_info "Report saved to: $output_file"
    fi
}

# -----------------------------------------------------------------------------
# Event Management
# -----------------------------------------------------------------------------

##
# Lists available perf events on the system.
#
# Arguments:
#   $1 - (Optional) Event type filter (e.g., "hw", "sw", "cache")
#
# Usage:
#   list_perf_events
#   list_perf_events "hw"
##
list_perf_events() {
    local event_type="${1:-}"
    
    validate_perf_installation
    
    if [[ -n "$event_type" ]]; then
        log_info "Available $event_type events:"
        /usr/local/bin/perf list "$event_type"
    else
        log_info "Available perf events:"
        /usr/local/bin/perf list
    fi
}

##
# Validates that specified perf events are available on the system.
#
# Arguments:
#   $@ - List of perf events to validate
#
# Usage:
#   validate_perf_events "cpu-cycles" "cache-misses" "branch-misses"
##
validate_perf_events() {
    local events=("$@")
    local unavailable_events=()
    
    validate_perf_installation
    
    for event in "${events[@]}"; do
        if ! /usr/local/bin/perf list | grep -q "$event"; then
            unavailable_events+=("$event")
        fi
    done
    
    if [ ${#unavailable_events[@]} -gt 0 ]; then
        handle_error "Unavailable perf events: ${unavailable_events[*]}"
    fi
    
    log_info "All specified perf events are available."
}

# -----------------------------------------------------------------------------
# Data Management
# -----------------------------------------------------------------------------

##
# Manages perf data file cleanup with user confirmation.
# Offers to save the data with a timestamp or delete it.
#
# Arguments:
#   $1 - Perf data file path
#   $2 - (Optional) Description for user prompt
#
# Usage:
#   manage_perf_data_cleanup "perf.data" "performance data"
##
manage_perf_data_cleanup() {
    local perf_data="$1"
    local description="${2:-perf data}"
    
    if [[ ! -f "$perf_data" ]]; then
        log_warning "Perf data file '$perf_data' not found. Nothing to clean up."
        return 0
    fi
    
    if prompt_yes_no "Do you want to preserve the current $description?" "n"; then
        local backup_name
        backup_name=$(generate_backup_name "$perf_data")
        mv "$perf_data" "$backup_name"
        log_info "$description saved as '$backup_name'."
    else
        rm -f "$perf_data"
        log_info "$description deleted."
    fi
}

# -----------------------------------------------------------------------------
# Installation Validation
# -----------------------------------------------------------------------------

##
# Validates that perf is properly installed and accessible.
#
# Usage:
#   validate_perf_installation
##
validate_perf_installation() {
    validate_tool_path "perf" "/usr/local/bin/perf"
    
    # Check if perf can run basic commands
    if ! /usr/local/bin/perf --version >/dev/null 2>&1; then
        handle_error "Perf is installed but not working properly."
    fi
    
    log_info "Perf installation validated successfully."
}

##
# Validates that pprof is properly installed and accessible.
#
# Usage:
#   validate_pprof_installation
##
validate_pprof_installation() {
    validate_tool_path "pprof" "/root/go/bin/pprof"
    
    # Check if pprof can run basic commands
    if ! /root/go/bin/pprof --help >/dev/null 2>&1; then
        handle_error "Pprof is installed but not working properly."
    fi
    
    log_info "Pprof installation validated successfully."
}

##
# Sets up the environment for perf_data_converter if available.
#
# Usage:
#   setup_perf_data_converter
##
setup_perf_data_converter() {
    local converter_path="/root/perf_data_converter/bazel-bin/src"
    
    if [[ -d "$converter_path" ]]; then
        export PATH="$converter_path:$PATH"
        log_info "Perf data converter added to PATH: $converter_path"
    else
        log_warning "Perf data converter not found at: $converter_path"
    fi
}
