#!/bin/bash
# ----------------------------------------------------------------------------
# This script automates benchmarking for the GGUI project using perf, pprof,
# and perf_data_converter. It supports various profiling events and launches
# a web interface for analysis.
#
# Usage:
#   $0 [OPTION]
#
# Options:
#   -h, --help            Display this help message.
#   -b, --branch          Profile using branch-misses event (default).
#   -c, --cycles          Profile using cpu-cycles event.
#   -i, --instructions    Profile using instructions event.
#   -a, --all             Profile using branch-misses, cpu-cycles, and instructions.
#   -F, -f                Enable maximum profiling (alias for --all with extra options).
#
# ----------------------------------------------------------------------------

# Function to display the help message.
show_help() {
    echo "Usage: $0 [OPTION]"
    echo "Benchmarking script for the GGUI project using perf, pprof, and perf_data_converter."
    echo
    echo "Options:"
    echo "  -h, --help            Display this help message."
    echo "  -b, --branch          Profile using branch-misses event (default)."
    echo "  -c, --cycles          Profile using cpu-cycles event."
    echo "  -i, --instructions    Profile using instructions event."
    echo "  -a, --all             Profile using branch-misses, cpu-cycles, and instructions."
    echo "  -F, -f                Enable maximum profiling (alias for --all with extra options)."
    exit 0
}

# Function to handle errors gracefully.
handle_error() {
    echo "Error: $1" >&2
    exit 1
}

# Process help option.
if [[ "$1" =~ ^(-h|--help)$ ]]; then
    show_help
fi

# Default profiling settings.
PERF_EVENT="branch-misses"
EXTRA_OPTIONS=""

# Check for profiling option flags.
if [[ "$1" =~ ^(-F|-f)$ ]]; then
    # Maximum profiling: combine several events and add extra options.
    PERF_EVENT="branch-misses,cpu-cycles,instructions"
    EXTRA_OPTIONS="--freq=1000"
elif [[ "$1" =~ ^(-c|--cycles)$ ]]; then
    PERF_EVENT="cpu-cycles"
elif [[ "$1" =~ ^(-i|--instructions)$ ]]; then
    PERF_EVENT="instructions"
elif [[ "$1" =~ ^(-a|--all)$ ]]; then
    PERF_EVENT="branch-misses,cpu-cycles,instructions"
elif [[ "$1" =~ ^(-b|--branch)$ ]]; then
    PERF_EVENT="branch-misses"
fi

# Step 1: Verify and build the project.
current_dir=$(pwd)
build_script="$current_dir/bin/build.sh"

if [[ ! -f "$build_script" ]]; then
    handle_error "Build script '$build_script' not found."
fi

echo "Building the project..."
"$build_script" || handle_error "Build process failed."

# Optional pause to allow user to review output before starting profiling.
echo "Starting benchmarking... You have a few seconds to review before profiling starts."
sleep 3

# Step 2: Run the application with perf recording.
echo "Profiling GGUI using perf with event(s): $PERF_EVENT"
# The -g flag enables call-graph (stack trace) recording.
# The -o option directs the output to a file named 'perf.data'.
/usr/local/bin/perf record -e "$PERF_EVENT" -g $EXTRA_OPTIONS -o perf.data "$current_dir/bin/Build/GGUI" \
    || handle_error "Perf recording failed."

# Step 3: Configure the environment for the perf_data_converter.
# (Assuming bazel export is available at the specified location)
export PATH="/root/perf_data_converter/bazel-bin/src:$PATH"

# Step 4: Launch pprof's web interface for analyzing the profile.
# The command syntax typically requires the target binary and the profiling data.
export BROWSER=wslview
echo "Launching pprof web interface on port 8080..."
/root/go/bin/pprof -http=:8080 "$current_dir/bin/Build/GGUI" perf.data \
    || handle_error "Failed to launch pprof analysis."

# Step 5: Ask the user whether to preserve or delete the profiling output.
echo "Do you want to preserve the current perf data? [Y/n]"
read -r answer

if [[ "$answer" =~ ^(Y|y)$ ]]; then
    current_date=$(date '+%Y-%m-%d_%H-%M-%S')
    mv perf.data "perf.data.$current_date.backup"
    echo "Perf data saved as 'perf.data.$current_date.backup'."
else
    rm -f perf.data
    echo "Perf data deleted."
fi
