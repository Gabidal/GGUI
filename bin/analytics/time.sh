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

# Parse common options (-h/-t/etc.) similar to benchmark.sh
parse_common_options "$@"

# Show help if requested
if [[ "$HELP_REQUESTED" == "true" ]]; then
    show_help
fi

# Extract positional numeric args (time_short, time_long) and script-specific options
EMIT_PREFIX=""
NUM_ARGS=()
ARGS=("$@")
i=0
while [[ $i -lt ${#ARGS[@]} ]]; do
    arg="${ARGS[$i]}"
    case "$arg" in
        --emit-callgrind-prefix)
            if [[ $((i+1)) -lt ${#ARGS[@]} ]]; then
                EMIT_PREFIX="${ARGS[$((i+1))]}"
                i=$((i+1))
            else
                handle_error "--emit-callgrind-prefix requires a value"
            fi
            ;;
        -h|--help|--h|-help)
            show_help
            ;;
        --type|-t|--type=*|-t=*)
            # handled by parse_common_options; skip
            ;;
        release|debug|type=release|type=debug|-t=release|-t=debug|--type=release|--type=debug)
            # already handled by parse_common_options; skip these tokens
            ;;
        *)
            if [[ "$arg" =~ ^[0-9]+$ ]]; then
                NUM_ARGS+=("$arg")
            fi
            ;;
    esac
    i=$((i+1))
done

if [[ ${#NUM_ARGS[@]} -lt 2 ]]; then
    show_help
fi

# Parse timing arguments
TIME_SHORT="${NUM_ARGS[0]}"
TIME_LONG="${NUM_ARGS[1]}"

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

ensure_wrappers_built() {
    ensure_bin_directory

    local BIN_DIR
    BIN_DIR="$(pwd)"              # .../GGUI/bin
    local BUILD_DIR
    BUILD_DIR="$(get_build_dir_for_type "$BUILD_TYPE")"

    # Ensure build directory is configured for the selected build type
    meson_setup_or_reconfigure "$BUILD_DIR" "$BIN_DIR" "$BUILD_TYPE"

    # Build the selected configuration via Meson so the executables exist
    compile_meson_build "$BUILD_TYPE"

    # Build the wrapper executables explicitly to be safe
    meson_build_targets "$BUILD_DIR" timingStanding timingBusy

    # Paths to built executables inside build dir
    STANDING_EXE="$BUILD_DIR/timingStanding"
    BUSY_EXE="$BUILD_DIR/timingBusy"

    if [ ! -x "$STANDING_EXE" ] || [ ! -x "$BUSY_EXE" ]; then
        handle_error "Failed to locate built executables in $BUILD_DIR"
    fi

    log_info "Wrappers built: $STANDING_EXE, $BUSY_EXE"
}

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
    local label="${3:-run}"

    local output_file
    if [[ -n "$EMIT_PREFIX" ]]; then
        output_file="${EMIT_PREFIX}_${label}.out"
    else
        output_file="callgrind_temp.out"
    fi

    log_info "Measuring instruction count for ${duration}s execution (label='${label}')..."

    # Run timed Callgrind profiling
    run_callgrind_timed "$duration" "$executable" "full" "$output_file"

    # Extract instruction count
    INSTRUCTION_COUNT=$(extract_instruction_count "$output_file")

    # Clean up temporary file (only when not emitting persistent files)
    if [[ -z "$EMIT_PREFIX" ]]; then
        rm -f "$output_file"
    else
        log_info "Saved callgrind profile: $output_file"
    fi

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
    echo "Short run (${short_time}s):    $short_count instructions"
    echo "Long run (${long_time}s):      $long_count instructions"

    local diff
    diff=$(echo "$long_count - $short_count" | bc)
    echo "Opcode difference (long - short): $diff"
    echo ""
    echo "Analysis Metrics:"
    echo "Time 1 (Instructions/sec):     $slope1"
    echo "Time 2 (Additional rate):      $slope2"
    echo "Growth Ratio:                  $ratio"
    echo ""
    
    # Provide interpretation
    echo "Interpretation:"
    if (( $(echo "$ratio > 1.1" | bc -l) )); then
        log_warning "High growth ratio detected - possible memory leak or performance degradation"
    elif (( $(echo "$ratio < 0.9" | bc -l) )); then
        log_info "Negative growth detected - possible optimization or reduced workload over time"
    else
        log_info "Stable performance - ratio close to zero indicates consistent behavior"
    fi
    echo
    echo "----------------------------------------------------------------------"
    echo
    echo
}

# =============================================================================
# Main Execution
# =============================================================================

# Setup environment and build wrappers
log_info "Setting up environment for performance growth analysis..."
ensure_wrappers_built

# Validate required tools
validate_valgrind_installation
validate_tools "timeout" "bc" "callgrind_annotate"

log_info "Performance growth analysis configuration:"
log_info "Short run duration: ${TIME_SHORT}s"
log_info "Long run duration:  ${TIME_LONG}s"
log_info "Build type:         ${BUILD_TYPE}"
log_info "Standing executable: $STANDING_EXE"
log_info "Busy executable:     $BUSY_EXE"

# Helper to compute ratio value (as string) without printing full analysis
compute_ratio_only() {
    local short_count="$1" long_count="$2" short_time="$3" long_time="$4"
    local slope1 slope2 ratio
    slope1=$(echo "scale=10; $short_count / $short_time" | bc -l)
    slope2=$(echo "scale=10; ($long_count - $short_count) / ($long_time - $short_time)" | bc -l)
    ratio=$(echo "scale=10; $slope2 / $slope1" | bc -l)
    echo "$ratio"
}

# Measure STANDING
log_info "[Standing] Short duration measurement..."
measure_instruction_count "$TIME_SHORT" "$STANDING_EXE" "standing_short"
standing_short="$INSTRUCTION_COUNT"

log_info "[Standing] Long duration measurement..."
measure_instruction_count "$TIME_LONG" "$STANDING_EXE" "standing_long"
standing_long="$INSTRUCTION_COUNT"

# Measure BUSY
log_info "[Busy] Short duration measurement..."
measure_instruction_count "$TIME_SHORT" "$BUSY_EXE" "busy_short"
busy_short="$INSTRUCTION_COUNT"

log_info "[Busy] Long duration measurement..."
measure_instruction_count "$TIME_LONG" "$BUSY_EXE" "busy_long"
busy_long="$INSTRUCTION_COUNT"

echo
echo "===== Standing: Detailed Analysis ====="
calculate_growth_analysis "$standing_short" "$standing_long" "$TIME_SHORT" "$TIME_LONG"
standing_ratio=$(compute_ratio_only "$standing_short" "$standing_long" "$TIME_SHORT" "$TIME_LONG")

echo
echo "===== Busy: Detailed Analysis ====="
calculate_growth_analysis "$busy_short" "$busy_long" "$TIME_SHORT" "$TIME_LONG"
busy_ratio=$(compute_ratio_only "$busy_short" "$busy_long" "$TIME_SHORT" "$TIME_LONG")

# Final summary ratios
echo
echo "================ Summary Ratios ================"
printf "Standing growth ratio (long vs short): %s\n" "$standing_ratio"
printf "Busy growth ratio (long vs short):     %s\n" "$busy_ratio"

# Compare busy vs standing to give sense of opcode growth differences
if command -v bc >/dev/null 2>&1; then
    if (( $(echo "$standing_ratio == 0" | bc -l) )); then
        comp_ratio="inf"
    else
        comp_ratio=$(echo "scale=10; $busy_ratio / $standing_ratio" | bc -l)
    fi
    printf "Relative (busy/standing) ratio:        %s\n" "$comp_ratio"
fi

log_info "Performance growth analysis completed."
