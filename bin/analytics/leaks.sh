#!/usr/bin/env bash

# =============================================================================
# GGUI Valgrind Memory Analysis Script
# =============================================================================
# This script performs comprehensive memory analysis for the GGUI project using
# Valgrind's Memcheck tool. It detects memory leaks, buffer overflows, stack
# overflows, and other memory-related issues with detailed reporting.
#
# Features:
# - Memory leak detection with full stack traces
# - Buffer overflow and underflow detection
# - Use-after-free and double-free detection
# - Stack overflow detection
# - Detailed logging with categorized error reporting
# - Minimal and comprehensive analysis modes
#
# Author: GGUI Analytics Team
# Version: 2.0 (Refactored with modular utilities)
# =============================================================================

# Source utility modules
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils/common.sh"
source "$SCRIPT_DIR/utils/valgrind.sh"
source "$SCRIPT_DIR/utils/help.sh"

# Function to display help message
show_help() {
    generate_memory_help "$(basename "$0")"
    exit 0
}

# Parse command line arguments
parse_common_options "$@"

# Display help if requested
if [[ "$HELP_REQUESTED" == "true" ]]; then
    show_help
fi

# =============================================================================
# Main Execution
# =============================================================================

# Setup environment and build project
log_info "Setting up environment for memory analysis..."
ensure_bin_directory
executable=$(ensure_executable)

# Validate Valgrind installation
validate_valgrind_installation

# Configure log file
log_file="valgrind.log"
analysis_mode="$PROFILING_MODE"

log_info "Memory analysis mode: $analysis_mode"
log_info "Log file: $log_file"

# Run memory analysis
run_memcheck_analysis "$executable" "$analysis_mode" "$log_file"

# Display results summary
if [[ -f "$log_file" && "$analysis_mode" == "minimal" ]]; then
    log_info "Memory analysis completed. Results are logged in '$log_file'."
    log_info "For detailed analysis, run with -F flag for full memory checking."
else
    log_info "Memory analysis completed successfully."
fi

# Check if there were any significant issues found
if [[ -f "$log_file" ]]; then
    error_count=$(grep -c "ERROR SUMMARY" "$log_file" 2>/dev/null || echo "0")
    if [[ "$error_count" -gt 0 ]]; then
        log_warning "Memory issues detected. Please review '$log_file' for details."
    else
        log_info "No memory errors detected in the analysis."
    fi
fi
