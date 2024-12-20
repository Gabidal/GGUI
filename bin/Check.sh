#!/bin/bash

# ----------------------------------------------------------------------------
# This script performs memory leak and stack overflow detection using Valgrind.
# It ensures the executable exists or is built, runs Valgrind, and logs detailed 
# results to a log file.
# ----------------------------------------------------------------------------

# Define constants for paths and log file.
LOG_FILE="valgrind.log"
EXECUTABLE_PATH="$(pwd)/bin/Build/GGUI"
BUILD_SCRIPT="$(pwd)/bin/Build.sh"
BUILD_DIR="$(pwd)/bin/Build"

# Function to handle errors and exit gracefully with an error message.
handle_error() {
    local error_message="$1"
    echo "Error: $error_message" >&2
    exit 1
}

# Check if the executable exists. If not, attempt to build it.
if [[ ! -f "$EXECUTABLE_PATH" ]]; then
    echo "Executable '$EXECUTABLE_PATH' not found."
    echo "Attempting to build..."
    
    if [[ ! -f "$BUILD_SCRIPT" ]]; then
        handle_error "Build script '$BUILD_SCRIPT' not found!"
    fi
    
    # Run the build script, or exit with an error if it fails.
    "$BUILD_SCRIPT" || handle_error "Build process failed."
fi

# Verify the build directory exists.
if [[ ! -d "$BUILD_DIR" ]]; then
    handle_error "Build directory '$BUILD_DIR' not found!"
fi

# Inform the user that Valgrind will run the executable and log results.
echo "Running Valgrind on '$EXECUTABLE_PATH' and logging results to '$LOG_FILE'..."

# Run Valgrind with enhanced memory leak and stack overflow checks.
valgrind --leak-check=full \
         --track-origins=yes \
         --show-leak-kinds=all \
         --log-file="$LOG_FILE" \
         "$EXECUTABLE_PATH"

# Check if Valgrind ran successfully and report the outcome.
if [[ $? -eq 0 ]]; then
    echo "Valgrind check completed successfully. Results are logged in '$LOG_FILE'."
else
    handle_error "Valgrind check failed. See '$LOG_FILE' for details."
fi
