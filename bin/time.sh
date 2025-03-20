#!/bin/bash
# ----------------------------------------------------------------------------
# This script automates performance benchmarking for a target application.
# It measures the number of instructions executed over two durations using
# Linux's "perf stat" tool and then computes:
#
#   - Slope1 = (instructions from short run) / TIME_SHORT
#   - Slope2 = (instructions from long run - instructions from short run) / (TIME_LONG - TIME_SHORT)
#   - Ratio  = Slope2 / Slope1
#
# The script also verifies and builds the project by invoking the build
# script located at "$current_dir/bin/build.sh".
#
# Usage:
#   ./measure_instructions.sh <program> [program_args...]
#
# Requirements:
#   - The "perf" utility must be installed and available at /usr/local/bin/perf.
#   - The "timeout" command must be available.
#
# ----------------------------------------------------------------------------

# Function to display the help message.
show_help() {
    echo "Usage: $0 time_a time_b"
    echo
    echo "This script runs the specified program for two durations and measures"
    echo "the number of instructions executed using 'perf stat'. It computes:"
    echo "  - Slope1: Instructions per second during the short run (TIME_SHORT)."
    echo "  - Slope2: Additional instructions per second during the extended run."
    echo "  - Ratio : Slope2 / Slope1, indicating the relative change in instruction rate."
    exit 0
}

# Function to handle errors gracefully.
handle_error() {
    echo "Error: $1" >&2
    exit 1
}

# Process help option or missing argument.
if [[ "$#" -lt 1 || "$1" =~ ^(-h|--help)$ ]]; then
    show_help
fi

# Step 1: Verify and build the project.
current_dir=$(pwd)
build_script="$current_dir/bin/build.sh"

if [[ ! -f "$build_script" ]]; then
    handle_error "Build script '$build_script' not found."
fi

echo "Building the project..."
"$build_script" || handle_error "Build process failed."

# Retrieve the program and its arguments.
PROGRAM="$current_dir/bin/Build/GGUI"

# Define run durations (in seconds).
TIME_SHORT=$1           # Short run duration.
TIME_LONG=$2            # Long run duration.

# Verify that the perf utility exists.
if [[ ! -x /usr/local/bin/perf ]]; then
    handle_error "'/usr/local/bin/perf' is not installed or not executable."
fi

get_instruction_count() {
    # Use awk to find the header line starting with "Ir", then read the next line after the next line.
    # Remove commas from the first field and print it.
    callgrind_annotate callgrind.out | awk '/^Ir/ {getline; getline; gsub(/,/, "", $1); print $1; exit}' | tail -n1 | tr -d '[:space:]'
}

RESULT=""
timer() {
    local runFor=$1
    echo "Running program '$PROGRAM' under Valgrind for a duration of ${runFor}s..."

    # Run the program inside Valgrind's callgrind tool
    # timeout "$runFor" valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes --simulate-cache=yes --collect-systime=yes --branch-sim=yes --callgrind-out-file=callgrind.out "$PROGRAM" 2>&1
    valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes --simulate-cache=yes --collect-systime=yes --branch-sim=yes --callgrind-out-file=callgrind.out "$PROGRAM" 2>&1

    # Extract total instruction count
    result=$(get_instruction_count)

    # rm -f callgrind.out

    if [ -z "$result" ]; then
        handle_error "Failed to retrieve instruction count for '$PROGRAM' after running for '$runFor' seconds."
    fi

    RESULT="$result"
}

# Step 2: Run the program for the short duration and capture instruction count.
timer $TIME_SHORT
SHORT_COUNT="$RESULT"
echo "Short run instructions: $SHORT_COUNT"

# Step 3: Run the program for the longer duration and capture instruction count.
timer $TIME_LONG
LONG_COUNT="$RESULT"
echo "Long run instructions: $LONG_COUNT"

# Step 4: Compute the instruction rate slopes.
SLOPE1=$(echo "scale=10; $SHORT_COUNT / $TIME_SHORT" | bc -l)

if [ $(($TIME_LONG - $TIME_SHORT)) -eq 0 ]; then
    echo "Error: TIME_LONG and TIME_SHORT are equal. Cannot compute SLOPE2."
else
    SLOPE2=$(echo "scale=10; ($LONG_COUNT - $SHORT_COUNT) / ($TIME_LONG - $TIME_SHORT)" | bc -l)

    RATIO=$(echo "scale=10; $SLOPE2 / $SLOPE1" | bc -l)

    # Step 5: Output the computed results.
    echo "-------------------------------"
    echo "Results:"
    echo "Time 1: $SLOPE1"
    echo "Time 2: $SLOPE2"
    echo "Growth: $RATIO"
fi
