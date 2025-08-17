#!/usr/bin/env bash

# =============================================================================
# GGUI Performance Growth Analysis Script
# =============================================================================
# This script measures instruction execution over two different time periods
# to analyze performance characteristics and detect potential issues like
# memory leaks or performance degradation over time.
#
# The analysis computes:
# - Slope1: Instructions per second during the short run
# - Slope2: Additional instructions per second during extended execution
# - Ratio: Slope2 / Slope1, indicating performance change over time
#
# A ratio significantly different from 1.0 may indicate:
# - Memory leaks (increasing instruction counts)
# - Performance degradation
# - Initialization overhead vs. steady-state performance
# =============================================================================

# Source utility modules
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils/common.sh"
source "$SCRIPT_DIR/utils/valgrind.sh"
source "$SCRIPT_DIR/utils/help.sh"

# Function to display help message
show_help() {
    generate_timing_help "$(basename "$0")"
    exit 0
}

# Validate arguments
if [[ "$#" -lt 2 || "$1" =~ ^(-h|--help)$ ]]; then
    show_help
fi

# Parse timing arguments
TIME_SHORT="$1"
TIME_LONG="$2"

# Validate numeric arguments
if ! [[ "$TIME_SHORT" =~ ^[0-9]+$ ]] || ! [[ "$TIME_LONG" =~ ^[0-9]+$ ]]; then
    handle_error "Time arguments must be positive integers (seconds)"
fi

if [[ "$TIME_LONG" -le "$TIME_SHORT" ]]; then
    handle_error "Long time ($TIME_LONG) must be greater than short time ($TIME_SHORT)"
fi

# =============================================================================
# Performance Measurement Functions
# =============================================================================

##
# Runs a timed instruction count measurement using Valgrind Callgrind.
#
# Arguments:
#   $1 - Duration in seconds
#   $2 - Executable path
#
# Sets global variable:
#   INSTRUCTION_COUNT - The measured instruction count
##
measure_instruction_count() {
    local duration="$1"
    local executable="$2"
    local temp_output="callgrind_temp.out"
    
    log_info "Measuring instruction count for ${duration}s execution..."
    
    # Run timed Callgrind profiling
    run_callgrind_timed "$duration" "$executable" "full" "$temp_output"
    
    # Extract instruction count
    INSTRUCTION_COUNT=$(extract_instruction_count "$temp_output")
    
    # Clean up temporary file
    rm -f "$temp_output"
    
    log_info "Instructions executed in ${duration}s: $INSTRUCTION_COUNT"
}

##
# Calculates and displays performance growth analysis.
#
# Arguments:
#   $1 - Short run instruction count
#   $2 - Long run instruction count
#   $3 - Short run duration
#   $4 - Long run duration
##
calculate_growth_analysis() {
    local short_count="$1"
    local long_count="$2"
    local short_time="$3"
    local long_time="$4"
    
    # Validate bc is available for calculations
    validate_tools "bc"
    
    # Calculate slopes
    local slope1=$(echo "scale=10; $short_count / $short_time" | bc -l)
    
    if [ $((long_time - short_time)) -eq 0 ]; then
        handle_error "Time intervals are equal. Cannot compute growth analysis."
    fi
    
    local slope2=$(echo "scale=10; ($long_count - $short_count) / ($long_time - $short_time)" | bc -l)
    local ratio=$(echo "scale=10; $slope2 / $slope1" | bc -l)
    
    # Display results
    echo "=================================="
    echo "Performance Growth Analysis Results"
    echo "=================================="
    echo "Short run (${short_time}s): $short_count instructions"
    echo "Long run (${long_time}s):  $long_count instructions"
    echo ""
    echo "Analysis Metrics:"
    echo "Time 1 (Instructions/sec):     $slope1"
    echo "Time 2 (Additional rate):      $slope2"
    echo "Growth Ratio:                  $ratio"
    echo ""
    
    # Provide interpretation
    echo "Interpretation:"
    if (( $(echo "$ratio > 0.1" | bc -l) )); then
        log_warning "High growth ratio detected - possible memory leak or performance degradation"
    elif (( $(echo "$ratio < -0.1" | bc -l) )); then
        log_info "Negative growth detected - possible optimization or reduced workload over time"
    else
        log_info "Stable performance - ratio close to zero indicates consistent behavior"
    fi
}

# =============================================================================
# Main Execution
# =============================================================================

# Setup environment and build project
log_info "Setting up environment for performance growth analysis..."
ensure_bin_directory
executable=$(ensure_executable)

# Validate required tools
validate_valgrind_installation
validate_tools "timeout" "bc" "callgrind_annotate"

log_info "Performance growth analysis configuration:"
log_info "Short run duration: ${TIME_SHORT}s"
log_info "Long run duration:  ${TIME_LONG}s"
log_info "Executable: $executable"

# Run first measurement (short duration)
log_info "Starting short duration measurement..."
measure_instruction_count "$TIME_SHORT" "$executable"
short_count="$INSTRUCTION_COUNT"

# Run second measurement (long duration)
log_info "Starting long duration measurement..."
measure_instruction_count "$TIME_LONG" "$executable"
long_count="$INSTRUCTION_COUNT"

# Calculate and display analysis
calculate_growth_analysis "$short_count" "$long_count" "$TIME_SHORT" "$TIME_LONG"

log_info "Performance growth analysis completed."
