#!/bin/bash

# This script prepares the environment and compiles a C++ project for the GGUI framework.

# Function to verify and navigate to the appropriate project directory.
check_directory() {
    local current_dir
    local project_root
    local project_root_name="GGUI"
    local bin_dir_name="Export"

    current_dir=$(pwd)
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)

    # Verify if the script is executed within a Git project.
    if [ -z "$project_root" ]; then
        echo "Error: Unable to locate the project root directory. Please ensure this script is executed within the GGUI project."
        exit 1
    fi

    # Navigate to the 'bin' directory if required.
    if [ "$(basename "$project_root")" == "$project_root_name" ] && [ "$current_dir" == "$project_root" ]; then
        echo "Detected project root. Navigating to the 'Export' directory..."
        cd "$project_root/$bin_dir_name" || exit 1
    elif [[ "$current_dir" != *"$project_root/$bin_dir_name"* ]]; then
        echo "Navigating to the 'Export' directory within the project..."
        cd "$project_root/$bin_dir_name" || exit 1
    fi
}

# Check if the required library libgit2-dev is installed.
verify_libgit2() {
    if [ ! -f /usr/include/git2.h ]; then
        echo "Error: libgit2-dev is not installed. Install it with:"
        echo "  sudo apt-get install libgit2-dev"
        exit 1
    fi
}

# Download dependencies if they are not present.
download_dependencies() {
    local url_json="https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
    local url_cpr="https://github.com/whoshuu/cpr/archive/refs/heads/master.zip"

    # Ensure json.hpp is downloaded.
    if [ ! -f json.hpp ]; then
        echo "Downloading json.hpp..."
        curl -L -o json.hpp "$url_json"
        if [ $? -ne 0 ]; then
            echo "Error: Failed to download json.hpp."
            exit 1
        fi
        echo "json.hpp downloaded successfully."
    else
        echo "json.hpp already exists."
    fi

    # Ensure CPR library is installed.
    if [ ! -d /usr/local/include/cpr ]; then
        echo "Downloading CPR library..."
        curl -L -o cpr.zip "$url_cpr"
        if [ $? -ne 0 ]; then
            echo "Error: Failed to download CPR library."
            exit 1
        fi
        echo "CPR library downloaded successfully."

        # Install unzip if not available.
        if ! command -v unzip &>/dev/null; then
            echo "Installing unzip utility..."
            sudo apt-get install -y unzip
        fi

        # Extract and build the CPR library.
        echo "Extracting and installing CPR library..."
        unzip cpr.zip -d cpr
        cd cpr/cpr-master || exit 1
        mkdir -p build && cd build

        # Install cmake if not available.
        if ! command -v cmake &>/dev/null; then
            echo "Installing CMake..."
            sudo apt-get install -y cmake
        fi

        cmake ..
        make
        sudo make install
        cd ../../../
    else
        echo "CPR library is already installed."
    fi
}

# Compile the project.
compile_project() {
    echo "Compiling the project..."
    g++ -g -o Export_Git ./Export_Git.cpp -I/usr/include -I/usr/local/include -lcpr -lgit2 -std=c++17
}

# Run the compiled program using gdb.
run_with_gdb() {
    cd ..   # Go back to the parent directory.
    echo "Launching the program with gdb..."
    # gdb --args ./Export/Export_Git ./ Dev
    ./Export/Export_Git ./ Dev
}

# CUDA_VISIBLE_DEVICES=0 python3 -m transformers.models.llama.convert_llama_weights_to_hf --model_size 8B --input_dir ./ --output_dir ./ --llama_version 3.1

# Main script execution.
main() {
    check_directory
    verify_libgit2
    download_dependencies
    compile_project
    run_with_gdb
}

main
