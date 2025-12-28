#!/usr/bin/env bash

# =============================================================================
# GGUI Valgrind Callgrind Profiling Script
# =============================================================================
# This script automates CPU profiling for the GGUI project using Valgrind's
# Callgrind tool. It provides comprehensive call graph analysis, instruction-
# level profiling, and optional cache simulation for detailed performance
# analysis.
#
# Features:
# - Automated project building and environment setup
# - Multiple profiling modes (basic and comprehensive)
# - KCachegrind integration for visual analysis
# - Profile data management with backup options
# - DRM mode support for hardware acceleration testing
#
# =============================================================================

# Source utility modules
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils/common.sh"
source "$SCRIPT_DIR/utils/valgrind.sh"
source "$SCRIPT_DIR/utils/help.sh"

# Function to display help message
show_help() {
    generate_benchmark_help
    exit 0
}

# Parse command line arguments (sets HELP_REQUESTED, PROFILING_MODE, ENABLE_DRM, BUILD_TYPE)
parse_common_options "$@"

# Display help if requested
if [[ "$HELP_REQUESTED" == "true" ]]; then
    show_help
fi

# =============================================================================
# Main Execution
# =============================================================================

# Setup environment and build project
log_info "Setting up environment for Callgrind profiling..."
ensure_bin_directory
# Build the selected configuration via Meson so the executable exists
compile_meson_build "$BUILD_TYPE"
# Select executable based on requested build type
preferred_exec="$(get_build_dir_for_type "$BUILD_TYPE")/ggui"
executable=$(ensure_executable "$preferred_exec")

# Validate Valgrind installation
validate_valgrind_installation

# User preparation countdown
countdown_timer 3 "Starting Callgrind profiling in"

# Configure profiling mode based on arguments
output_file="callgrind.out"
log_info "Profiling mode: $PROFILING_MODE"
log_info "DRM mode: ${ENABLE_DRM:-disabled}"

# Run Callgrind profiling
run_callgrind_profile "$preferred_exec" "$PROFILING_MODE" "$output_file" "$ENABLE_DRM"

# Open results in KCachegrind for analysis
open_profile_in_kcachegrind "$output_file"

# Manage profile cleanup with user confirmation
manage_profile_cleanup "$output_file" "Callgrind profile"

log_info "Callgrind profiling session completed."
