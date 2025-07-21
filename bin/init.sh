#!/bin/bash

# ----------------------------------------------------------------------------
# Initializes and builds the GGUI project locally. Ensures the script is run
# from the project root directory, checks for required tools (meson, g++), and
# manages the build setup and compilation process.
# ----------------------------------------------------------------------------

# Function to check if the script is being run from the project root directory
check_directory() {
    local current_dir=$(pwd)
    local project_root_name="GGUI"
    local bin_dir_name="bin"

    # Attempt to find the project root directory by looking for the .git directory
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)

    # If the project root is not found, exit with an error message
    if [ -z "$project_root" ]; then
        echo "Error: Unable to determine the project root directory. Ensure you're in the GGUI project."
        exit 1
    fi

    # Navigate to the 'bin' directory if not already there
    if [ "$(basename "$project_root")" == "$project_root_name" ] && [ "$current_dir" == "$project_root" ]; then
        echo "Project root directory detected. Changing to the 'bin' directory."
        cd "$project_root/$bin_dir_name" || exit 1
    elif [[ "$current_dir" != "$project_root/$bin_dir_name"* ]]; then
        echo "Navigating to the 'bin' directory within the project."
        cd "$project_root/$bin_dir_name" || exit 1
    fi
}

# Function to check if a command is available and exit if not
check_command() {
    local cmd="$1"
    local cmd_name="$2"

    if ! command -v "$cmd" &>/dev/null; then
        echo "Error: '$cmd_name' is required but not installed. Please install it before running this script."
        exit 1
    fi
}

# Check if we're in the correct directory
check_directory

# Ensure the required tools are installed
check_command "meson" "meson"
check_command "g++" "g++"

# Ensure the CXX variable is set (default to 'g++' if not)
if [ -z "$CXX" ]; then
    export CXX=g++
    echo "CXX environment variable was not set. Defaulting to 'g++'."
fi

# Step 1: Set up the build directory (wipe existing if necessary)
echo "Setting up the build directory..."
if [ -d "./build" ]; then
    meson setup --wipe build || exit 1
else
    meson setup build || exit 1
fi

# Step 2: Compile the project using meson
echo "Compiling the project..."
meson compile -C build || exit 1

# Step 3: Ensure necessary scripts have the correct permissions
echo "Setting execution permissions on build scripts..."
chmod 755 build.sh ./analytics/benchmark* ./analytics/check.sh ./analytics/time.sh || exit 1

# Completion message
echo "build process completed successfully!"
