#!/bin/bash

# Initializes and builds GGUI locally.

# Function to check if the script is run from the project root or a subdirectory
check_directory() {
    local current_dir=$(pwd)
    local project_root_name="GGUI"
    local bin_dir_name="bin"

    # Find the project root directory by looking for the .git directory
    project_root=$(git rev-parse --show-toplevel 2> /dev/null)

    # If git is not found or the project root is not determined
    if [ -z "$project_root" ]; then
        echo "Unable to determine the project root directory. Make sure you're in the GGUI project."
        exit 1
    fi

    # Check if the current directory is the project root
    if [ "$(basename "$project_root")" == "$project_root_name" ] && [ "$current_dir" == "$project_root" ]; then
        echo "In the project root directory. Changing to 'bin' directory."
        cd "$project_root/$bin_dir_name"
    elif [[ "$current_dir" != *"$project_root/$bin_dir_name"* ]]; then
        echo "Changing to the 'bin' directory within the project."
        cd "$project_root/$bin_dir_name"
    fi
}

# Call the check_directory function to ensure we're in the correct directory
check_directory

# Check if meson is installed or not
if ! command -v meson &> /dev/null
then
    echo "meson could not be found. Please install meson before running this script."
    exit 1
fi

# Check if g++ is installed or not
if ! command -v g++ &> /dev/null
then
    echo "g++ could not be found. Please install g++ before running this script."
    exit 1
fi

# Check if g++ is in CXX variable
if [ -z "$CXX" ]; then
    # Set CXX to the value of 'g++'
    CXX=g++
fi

# Check if there is a ./Build folder
if [ -d "./Build" ]; then
    meson setup --wipe Build
else
    meson setup Build
fi

# Now compile
meson compile -C Build

# Also set the Build.sh, Benchmark.sh into chmod 755
chmod 755 Build.sh
chmod 755 Benchmark.sh

echo "Done!"
