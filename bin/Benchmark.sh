#!/bin/bash

# ----------------------------------------------------------------------------
# This script automates profiling for the GGUI project using Valgrind (with 
# Callgrind) and then opens the results in KCachegrind for analysis.
# It provides an option to enable maximum profiling and gives the user an 
# opportunity to preserve or delete the profiling output.
# 
# Usage:
#   $0 [OPTION]
# 
# Options:
#   -F, -f     Enable maximum profiling (simulates cache, collects jumps, etc.).
#   -h, --help Display this help message.
# ----------------------------------------------------------------------------

# Function to display the help message.
show_help() {
    echo "Usage: $0 [OPTION]"
    echo "Profiling script for the GGUI project with Valgrind and Callgrind."
    echo
    echo "Options:"
    echo "  -F, -f     Enable maximum profiling (simulates cache, collects jumps, etc.)."
    echo "  -h, --help Display this help message."
    exit 0
}

# Function to handle errors and exit gracefully with a message.
handle_error() {
    echo "Error: $1" >&2
    exit 1
}

# Display help message if requested.
if [[ "$1" =~ ^(-help|--help|-h|--h)$ ]]; then
    show_help
fi

# Step 1: Verify and build the project.
echo "Building the project..."

current_dir=$(pwd)
build_script="$current_dir/bin/Build.sh"

if [[ ! -f "$build_script" ]]; then
    handle_error "Build script '$build_script' not found."
fi

"$build_script" || handle_error "Build process failed."

# Step 2: Allow user to read the output before starting profiling.
echo "Starting benchmark... You have a few seconds to read the output before profiling starts."
sleep 3

# Step 3: Determine the appropriate Valgrind profiling settings.
echo "Running GGUI with Valgrind and Callgrind enabled..."

DEFAULT_SETTINGS="--tool=callgrind --dump-instr=yes -s"
FULL_SETTINGS="--tool=callgrind --dump-instr=yes --collect-jumps=yes --simulate-cache=yes --collect-systime=yes"

# Select profiling settings based on user input.
if [[ "$1" =~ ^(-F|-f)$ ]]; then
    CURRENT_FLAG=$FULL_SETTINGS
else
    CURRENT_FLAG=$DEFAULT_SETTINGS
fi

# Step 4: Run the application with Valgrind profiling.
valgrind $CURRENT_FLAG --callgrind-out-file=callgrind.out "$current_dir/bin/Build/GGUI" || handle_error "Valgrind profiling failed."

# Step 5: Open the profiling results in KCachegrind.
echo "Opening the profile file with KCachegrind..."
kcachegrind callgrind.out || handle_error "Failed to open KCachegrind."

# Step 6: Ask the user whether to preserve or delete the profile output.
echo "Do you want to preserve the current profile? [Y/n]"
read -r answer

# Default action is to delete the file if no input is provided.
if [[ "$answer" =~ ^(Y|y)$ ]]; then
    current_date=$(date '+%Y-%m-%d_%H-%M-%S')
    mv callgrind.out "callgrind.out.$current_date.backup"
    echo "Profile saved as 'callgrind.out.$current_date.backup'."
else
    rm -f callgrind.out
    echo "Profile deleted."
fi
