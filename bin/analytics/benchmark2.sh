#!/usr/bin/env bash

# =============================================================================
# Automatic pprof with web UI benchmark.
# TODO: add c2c record for shared thread cache miss details
# =============================================================================

# Source utility modules
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils/common.sh"
source "$SCRIPT_DIR/utils/perf.sh"
source "$SCRIPT_DIR/utils/help.sh"

# Function to display help message
show_help() {
    generate_perf_help "$(basename "$0")"
    exit 0
}

# Parse command line arguments
parse_common_options "$@"
parse_perf_options "$@"

# Display help if requested
if [[ "$HELP_REQUESTED" == "true" ]]; then
    show_help
fi

# Setup environment and build project
log_info "Setting up environment for perf performance analysis..."
go_to_project_root
executable=$(ensure_executable "$BUILD_TYPE")

# Validate perf and pprof installations
validate_perf_installation
validate_pprof_installation

# Setup perf data converter environment if available
setup_perf_data_converter

# User preparation countdown
countdown_timer 3 "Starting perf profiling in"

# Configure output file
output_file="perf.data"
log_info "Performance events: $PERF_EVENT"
log_info "Profiling mode: $PERF_MODE"
if [[ -n "$EXTRA_OPTIONS" ]]; then
    log_info "Extra options: $EXTRA_OPTIONS"
fi

# Run perf recording
run_perf_record "$executable" "$PERF_MODE" "$output_file" "$PERF_EVENT" "$EXTRA_OPTIONS"

# Set browser for WSL compatibility if available
if command -v wslview >/dev/null 2>&1; then
    export BROWSER=wslview
    log_info "Using WSL browser integration"
fi

# Launch pprof web interface
launch_pprof_web "$executable" "$output_file" 8080

# Manage profile data cleanup with user confirmation
manage_perf_data_cleanup "$output_file" "performance data"

log_info "Perf performance analysis session completed."
