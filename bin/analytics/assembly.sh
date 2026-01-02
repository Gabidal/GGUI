#!/usr/bin/env bash

# =============================================================================
# GGUI Assembly Generation Script
# =============================================================================
# This script automates assembly code generation for the GGUI project using
# objdump disassembler. It provides comprehensive disassembly output with
# Intel syntax, source code interleaving, and symbol demangling for detailed
# binary analysis.
#
# Features:
# - Automated project building and environment setup
# - Multiple build type support (debug/release)
# - Intel syntax assembly output with source annotations
# - C++ symbol demangling for readability
# - Output file management with backup options
#
# =============================================================================

# Source utility modules
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils/common.sh"
source "$SCRIPT_DIR/utils/valgrind.sh"
source "$SCRIPT_DIR/utils/help.sh"

# Function to display help message
show_help() {
    generate_help_header "$(basename "$0")" "Generate assembly code from GGUI executable"
    
    cat << 'EOF'

Description:
  This script builds the GGUI executable and generates a comprehensive
  assembly listing using objdump. The output includes Intel-syntax
  disassembly with interleaved source code and demangled C++ symbols
  for enhanced readability and analysis.

Options:
  -h, --help           Display this help message and exit
  -t, --type=TYPE      Build type: 'debug' or 'release' (default: debug)
  -F, -f, --full       Generate full disassembly (all sections)
  --enableDRM          Enable DRM mode for GGUI (hardware acceleration)

Output:
  Creates 'assembly.asm' in the bin directory containing the
  complete disassembly of the GGUI executable.

EOF
    
    generate_help_footer
    exit 0
}

# Parse command line arguments (sets HELP_REQUESTED, PROFILING_MODE, ENABLE_DRM, BUILD_TYPE)
parse_common_options "$@"

# Display help if requested
if [[ "$HELP_REQUESTED" == "true" ]]; then
    show_help
fi

# =============================================================================
# Main Execution
# =============================================================================

# Setup environment and build project
log_info "Setting up environment for assembly generation..."
go_to_project_root
# Build the selected configuration via Meson so the executable exists
executable=$(ensure_executable "$BUILD_TYPE")

# Validate objdump installation
validate_tools "objdump"

# Configure output file
output_file="assembly.asm"
log_info "Build type: $BUILD_TYPE"
log_info "Executable: $executable"

# Prepare objdump arguments
objdump_args="-d -M intel -S --demangle"

# Add full disassembly mode if requested (via -F/--full flag)
if [[ "$PROFILING_MODE" == "full" ]]; then
    log_info "Disassembly mode: full (all sections)"
    objdump_args="$objdump_args -D"
else
    log_info "Disassembly mode: standard (executable sections only)"
fi

# Generate assembly output
log_info "Generating assembly output with objdump..."
if objdump $objdump_args "$executable" > "$output_file" 2>&1; then
    log_info "Assembly generation completed successfully"
    
    # Get absolute path for user-friendly output
    output_path="$(cd "$(dirname "$output_file")" && pwd)/$(basename "$output_file")"
    
    # Display file information
    file_size=$(du -h "$output_file" | cut -f1)
    line_count=$(wc -l < "$output_file")
    
    echo ""
    echo "======================================================================"
    echo "Assembly Output Generated"
    echo "======================================================================"
    echo "Location:    $output_path"
    echo "Size:        $file_size"
    echo "Lines:       $line_count"
    echo "Build Type:  $BUILD_TYPE"
    echo "======================================================================"
    echo ""
else
    handle_error "Failed to generate assembly output with objdump"
fi

# Manage output cleanup with user confirmation
manage_profile_cleanup "$output_file" "Assembly output"

log_info "Assembly generation session completed."
