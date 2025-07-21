#!/bin/bash

# ----------------------------------------------------------------------------
# Initializes and builds the GGUI project locally.
# Ensures the script is run from the project root directory, verifies required
# tools (meson, g++), and runs the compilation process.
# ----------------------------------------------------------------------------

# Function to check if the script is run from the project root directory or a subdirectory.
check_directory() {
    local current_dir=$(pwd)
    local project_root_name="GGUI"
    local bin_dir_name="bin"

    # Find the project root directory by looking for the .git directory
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)

    # If git is not found or the project root is not determined
    if [ -z "$project_root" ]; then
        echo "Error: Unable to determine the project root directory. Ensure you're in the GGUI project."
        exit 1
    fi

    # If we're in the project root, change to the 'bin' directory
    if [ "$(basename "$project_root")" == "$project_root_name" ] && [ "$current_dir" == "$project_root" ]; then
        echo "Project root directory detected. Changing to the 'bin' directory."
        cd "$project_root/$bin_dir_name" || exit 1
    # Otherwise, navigate to the 'bin' directory from anywhere in the project
    elif [[ "$current_dir" != *"$project_root/$bin_dir_name"* ]]; then
        echo "Navigating to the 'bin' directory within the project."
        cd "$project_root/$bin_dir_name" || exit 1
    fi
}

# Call the check_directory function to ensure we're in the correct directory
check_directory

# Function to check if a command is available, exit if not
check_command() {
    local cmd="$1"
    local cmd_name="$2"

    if ! command -v "$cmd" &>/dev/null; then
        echo "Error: '$cmd_name' is required but not installed. Please install it before running this script."
        exit 1
    fi
}

# Check if meson is installed
check_command "meson" "meson"

# Check if g++ is installed
check_command "g++" "g++"

# Ensure the CXX variable is set to the correct compiler (g++ if not set)
if [ -z "$CXX" ]; then
    export CXX=g++
    echo "CXX environment variable was not set. Defaulting to 'g++'."
fi

# Compile the project using meson
echo "Compiling the project..."
meson compile -C build || exit 1

# Completion message
echo "build completed successfully!"
