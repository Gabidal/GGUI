#!/usr/bin/env bash

# =============================================================================
# GGUI Project Initialization Script
# =============================================================================
# This script initializes and builds the GGUI project locally. It validates
# the environment, checks for required tools, manages the build setup, and
# ensures analytics tools are configured.
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/analytics/utils/common.sh"

# Reset location.
go_to_project_root

echo "=== GGUI Project Initialization ==="
echo

# Set execution permissions on all necessary scripts
echo "Setting execution permissions..."
find . -name "*.sh" -type f -exec chmod +x {} \; 2>/dev/null || true

# Step 1: Run analytics validation
echo "Validating environment and analytics tools..."
if bash "$SCRIPT_DIR/analytics/utils/validate.sh"; then
    echo "Environment validation completed successfully."
else
    echo "Environment validation failed. Please address the issues above."
    echo "You can continue with basic build, but analytics tools may not work properly."
    # Allow non-interactive/CI environments to bypass the prompt.
    # Conditions to auto-continue:
    #  - CI environment (GitHub Actions sets CI=true)
    if [[ -n "$CI" ]]; then
        echo "Force/CI mode detected (CI=$CI). Skipping question..." >&2
    else
        if ! prompt_yes_no "Continue regardless?"; then
            echo "Initialization aborted by user."
            exit 1
        fi
    fi
fi

echo

# Step 2: Set up build environment
echo "Setting up build environment..."

# Ensure the CXX variable is set (default to 'c++' if not)
if [ -z "$CXX" ]; then
    export CXX=c++
    echo "CXX environment variable was not set. Defaulting to 'c++'."
fi

# Check if ./build, ./build-win, ./build-release, ./build-linux exists, if so then remove them
for dir in "$SCRIPT_DIR"/build*/; do
    if [ -d "$dir" ]; then
        echo "Removing existing build directory: $dir"
        rm -rf "$dir"
    fi
done

# Check if ./export/*.a or ./export/*.lib files exist, if so then remove them
for file in "$SCRIPT_DIR"/export/*.{a,lib}; do
    if [ -e "$file" ]; then
        echo "Removing existing export file: $file"
        rm -f "$file"
    fi
done

# Check if ./export/ggui.h or ./export/ggui_dev.h exist, if so then remove them
for header in "$SCRIPT_DIR"/export/ggui*.h; do
    if [ -e "$header" ]; then
        echo "Removing existing export header: $header"
        rm -f "$header"
    fi
done

echo "Setting up the default build configure..."
meson_setup_or_reconfigure debug

# Step 3: Compile the project using meson
echo "Compiling the project..."
meson_compile_target debug ggui_core

echo
echo "=== Initialization Complete ==="
echo "Run ./bin/test.sh to test GGUI integrity"
echo "Run ./bin/export.sh to export GGUI as linkable libraries and auto generate headers"
echo "Run ./bin/analytics/* scripts for performance and memory leak checks"
