#!/usr/bin/env bash

# =============================================================================
# Help and Documentation Utility Functions for GGUI Analytics
# =============================================================================
# This module provides standardized help text generation and documentation
# utilities for analytics scripts. It ensures consistent formatting and
# comprehensive documentation across all analytics tools.
# =============================================================================

# Source common utilities
UTILS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -z "${COMMON_SOURCED:-}" ]]; then
    source "$UTILS_DIR/common.sh"
    COMMON_SOURCED=true
fi

# -----------------------------------------------------------------------------
# Help Text Generation
# -----------------------------------------------------------------------------

##
# Generates standardized help header for analytics scripts.
#
# Arguments:
#   $1 - Script name
#   $2 - Script description
#   $3 - (Optional) Usage pattern. Defaults to "$0 [OPTION]"
#
# Usage:
#   generate_help_header "benchmark.sh" "Valgrind profiling script"
##
generate_help_header() {
    local script_name="$1"
    local description="$2"
    local usage_pattern="${3:-\$0 [OPTION]}"
    
    cat << EOF
Usage: $usage_pattern
$description for the GGUI project.

EOF
}

##
# Generates standardized help footer with common information.
#
# Usage:
#   generate_help_footer
##
generate_help_footer() {
    cat << EOF

Environment:
  The script automatically detects the GGUI project root and navigates
  to the appropriate bin directory. It will build the project if needed.

Requirements:
  - Git repository (for project root detection)
  - GGUI project structure with bin/build.sh script
  - Appropriate analysis tools (varies by script)

Examples:
  Run with default settings:    \$0
  Show this help:               --help
  
For more information, see the GGUI project documentation.
EOF
}

valgrind_modular_help_text() {
    # Usage: valgrind_modular_help_text "ToolName"
    local tool_name="$1"

    cat << EOF
  -h, --help      Display this help message and exit.
  -F, -f          Enable maximum profiling with comprehensive analysis.
  --enableDRM     Enable DRM mode for GGUI (hardware acceleration).

Profiling Modes:
  Default:        Basic $tool_name analysis with standard settings.
  Full (-F/-f):   Comprehensive analysis with all available options.
                  Includes cache simulation, jump collection, and timing.

Output:
  Profile data is saved and can be opened in appropriate analysis tools.
  You will be prompted to save or delete the profile data after analysis.
EOF
}

##
# Generates help text for Valgrind-based tools.
#
# Arguments:
#   $1 - Script name
#   $2 - Tool name (e.g., "Callgrind", "Memcheck")
#
# Usage:
#   generate_valgrind_help "benchmark.sh" "Callgrind"
##
generate_valgrind_help() {
    local script_name="$1"
    local tool_name="$2"
    
    generate_help_header "$script_name" "Valgrind $tool_name analysis script"
    
    cat << EOF
Options:
$(valgrind_modular_help_text "$tool_name")
EOF
    
    generate_help_footer
}

##
# Generates help text for benchmark.sh.
#
# Arguments:
#   $1 - build type (e.g, "debug", "release")
#
# Usage:
#   generate_valgrind_help -t release
##
generate_benchmark_help() {
    generate_help_header "benchmark.sh" "GGUI Valgrind Callgrind Profiling Script"
    
    cat << EOF
Options:
  -t, --type=release
                  Specify the build type (debug or release, defaults to debug).
$(valgrind_modular_help_text "Callgrind")
EOF
    
    generate_help_footer
}

##
# Generates help text for Perf-based tools.
#
# Arguments:
#   $1 - Script name
#
# Usage:
#   generate_perf_help "benchmark2.sh"
##
generate_perf_help() {
    local script_name="$1"
    
    generate_help_header "$script_name" "Linux perf performance analysis script"
    
    cat << EOF
Options:
  -h, --help            Display this help message and exit.
  -b, --branch          Profile using branch-misses event (default).
  -c, --cycles          Profile using cpu-cycles event.
  -i, --instructions    Profile using instructions event.
  -a, --all             Profile using multiple events (branch-misses, cpu-cycles, instructions).
  -F, -f                Enable maximum profiling (alias for --all with extra options).

Profiling Events:
  branch-misses:   Measures branch prediction misses (default).
  cpu-cycles:      Measures CPU cycles consumed.
  instructions:    Measures instructions executed.
  all/maximum:     Combines multiple events for comprehensive analysis.

Analysis:
  Profile data is analyzed using pprof with a web interface.
  The web interface will open automatically on port 8080.

EOF
    
    generate_help_footer
}

##
# Generates help text for memory analysis tools.
#
# Arguments:
#   $1 - Script name
#
# Usage:
#   generate_memory_help "leaks.sh"
##
generate_memory_help() {
    local script_name="$1"
    
    generate_help_header "$script_name" "Memory leak and error detection script"
    
    cat << EOF
Options:
  -h, --help      Display this help message and exit.
  -F, -f          Enable full memory analysis (leak detection, stack traces).

Analysis Modes:
  Default:        Minimal checks focusing on stack problems and basic errors.
                  Faster execution with essential memory error detection.
  Full (-F/-f):   Comprehensive memory analysis including:
                  - Full leak detection
                  - Complete stack traces
                  - All memory error types
                  - Detailed reporting

Output:
  Results are logged to 'valgrind.log' in the current directory.
  The log contains detailed information about any memory issues found.

EOF
    
    generate_help_footer
}

##
# Generates help text for timing and performance measurement tools.
#
# Arguments:
#   $1 - Script name
#
# Usage:
#   generate_timing_help "time.sh"
##
generate_timing_help() {
    local script_name="$1"
    
    generate_help_header "$script_name" "Performance timing and instruction count analysis" "\$0 [OPTIONS] time_short time_long"
    
    cat << EOF
Arguments:
  time_short      Duration for short run (in seconds).
  time_long       Duration for long run (in seconds).

Options:
  -t, --type=release
                  Specify the build type (debug or release, defaults to debug).
  --emit-callgrind-prefix <prefix>
                  Save generated Callgrind outputs with the given prefix instead
                  of deleting them (files like <prefix>_standing_short.out).
  -h, --help      Display this help message and exit.

Description:
  This script measures instruction execution over two different durations
  and computes performance metrics:

  - Slope1: Instructions per second during the short run.
  - Slope2: Additional instructions per second during extended execution.
  - Ratio:  Slope2 / Slope1, indicating performance change over time.

  The analysis helps identify:
  - Performance degradation over time
  - Memory leaks (increasing instruction counts)
  - Initialization overhead vs. steady-state performance

Method:
  Uses Valgrind Callgrind to measure exact instruction counts during
  timed execution runs. The executable is run twice with timeout control.
  The build type controls whether wrappers are run from bin/build or
  bin/build-release.

Requirements:
  - Valgrind with Callgrind support
  - callgrind_annotate tool
  - timeout command
  - bc calculator

Examples:
  Basic run (debug build):      \$0 3 15
  Run using release build:      \$0 -t release 3 15
  Keep callgrind outputs:       \$0 3 15 --emit-callgrind-prefix runA

EOF
    
    generate_help_footer
}

# -----------------------------------------------------------------------------
# Option Parsing Utilities
# -----------------------------------------------------------------------------

##
# Parses common command line options and sets global variables.
# Handles standard options like help, verbose, and profiling modes.
#
# Arguments:
#   $@ - Command line arguments to parse
#
# Sets global variables:
#   HELP_REQUESTED - Set to "true" if help was requested
#   PROFILING_MODE - Set to "minimal", "full", or "custom"
#   VERBOSE_MODE - Set to "true" if verbose output requested
#   ENABLE_DRM - Set to "--enableDRM" if DRM mode requested
#
# Usage:
#   parse_common_options "$@"
#   if [[ "$HELP_REQUESTED" == "true" ]]; then
#       show_help
#   fi
##
parse_common_options() {
  HELP_REQUESTED="false"
  PROFILING_MODE="minimal"
  VERBOSE_MODE="false"
  ENABLE_DRM=""
  BUILD_TYPE="debug"    # default build type

  # Iterate with index to support two-arg flags like --type release
  local ARGS=("$@")
  local i=0
  while [[ $i -lt ${#ARGS[@]} ]]; do
    local arg="${ARGS[$i]}"
    case "$arg" in
      -h|--help|--h|-help)
        HELP_REQUESTED="true"
        ;;
      -F|-f|--full)
        PROFILING_MODE="full"
        ;;
      -v|--verbose)
        VERBOSE_MODE="true"
        ;;
      --enableDRM)
        ENABLE_DRM="--enableDRM"
        ;;
      # Build type options (several accepted forms)
      type=release|--type=release|-t=release|release)
        BUILD_TYPE="release"
        ;;
      type=debug|--type=debug|-t=debug|debug)
        BUILD_TYPE="debug"
        ;;
      type=profile|--type=profile|-t=profile|profile)
        BUILD_TYPE="profile"
        ;;
      --type|-t)
        # Look ahead for the next value
        if [[ $((i+1)) -lt ${#ARGS[@]} ]]; then
          local next="${ARGS[$((i+1))]}"
          if [[ "$next" == "release" || "$next" == "debug" || "$next" == "profile" ]]; then
            BUILD_TYPE="$next"
            i=$((i+1))
          fi
        fi
        ;;
    esac
    i=$((i+1))
  done
}

##
# Parses perf-specific command line options.
#
# Arguments:
#   $@ - Command line arguments to parse
#
# Sets global variables:
#   PERF_EVENT - The perf event to monitor
#   PERF_MODE - The profiling mode ("basic", "comprehensive", etc.)
#   EXTRA_OPTIONS - Additional perf options
#
# Usage:
#   parse_perf_options "$@"
##
parse_perf_options() {
    PERF_EVENT="branch-misses"
    PERF_MODE="basic"
    EXTRA_OPTIONS=""
    
    for arg in "$@"; do
        case "$arg" in
            -b|--branch)
                PERF_EVENT="branch-misses"
                PERF_MODE="basic"
                ;;
            -c|--cycles)
                PERF_EVENT="cpu-cycles"
                PERF_MODE="basic"
                ;;
            -i|--instructions)
                PERF_EVENT="instructions"
                PERF_MODE="basic"
                ;;
            -a|--all)
                PERF_EVENT="branch-misses,cpu-cycles,instructions"
                PERF_MODE="comprehensive"
                ;;
            -F|-f|--full)
                PERF_EVENT="branch-misses,cpu-cycles,instructions"
                PERF_MODE="comprehensive"
                EXTRA_OPTIONS="--freq=1000"
                ;;
        esac
    done
}

# -----------------------------------------------------------------------------
# Documentation Generation
# -----------------------------------------------------------------------------

##
# Generates a comprehensive usage guide for all analytics scripts.
#
# Usage:
#   generate_analytics_guide > ANALYTICS_GUIDE.md
##
generate_analytics_guide() {
    cat << 'EOF'
# GGUI Analytics Tools Guide

This directory contains various performance analysis and debugging tools for the GGUI project.

## Available Scripts

### benchmark.sh - Valgrind Callgrind Profiling
Comprehensive CPU profiling using Valgrind's Callgrind tool.

**Features:**
- Call graph generation
- Instruction-level analysis
- Cache simulation (with -F option)
- KCachegrind integration

**Usage:**
```bash
./benchmark.sh              # Basic profiling
./benchmark.sh -F            # Full profiling with cache simulation
./benchmark.sh --enableDRM   # Enable hardware acceleration
./benchmark.sh -type=release # You can enable this with other flags to test for release performance testing
```

### benchmark2.sh - Linux Perf Profiling
Modern performance profiling using Linux perf with web-based analysis.

**Features:**
- Multiple performance counters
- Branch prediction analysis
- CPU cycle measurement
- Web-based pprof interface

**Usage:**
```bash
./benchmark2.sh              # Branch misses analysis
./benchmark2.sh -c           # CPU cycles analysis
./benchmark2.sh -a           # Comprehensive analysis
```

### leaks.sh - Memory Analysis
Memory leak detection and stack overflow analysis using Valgrind Memcheck.

**Features:**
- Memory leak detection
- Stack overflow detection
- Use-after-free detection
- Detailed error reporting

**Usage:**
```bash
./leaks.sh                   # Basic memory checks
./leaks.sh -F                # Full memory analysis
```

### time.sh - Performance Growth Analysis
Measures instruction count growth over time to detect performance issues.

**Features:**
- Instruction count measurement
- Performance degradation detection
- Memory leak indicators
- Growth rate analysis

**Usage:**
```bash
./time.sh 5 30               # Compare 5s vs 30s runs
./time.sh 5 30 -t release    # Compare 5s vs 30s runs in release mode
```

## Utility Modules

The `utils/` directory contains reusable modules:

- **common.sh**: Shared utilities (error handling, directory management)
- **valgrind.sh**: Valgrind-specific operations
- **perf.sh**: Linux perf utilities
- **help.sh**: Documentation and help text generation

## Requirements

- Valgrind (with Callgrind and Memcheck)
- Linux perf tools
- KCachegrind (for visualization)
- pprof (for web analysis)
- Standard Unix tools (timeout, bc, etc.)

## Getting Started

1. Navigate to the GGUI project root
2. Run any script from anywhere in the project
3. Scripts automatically handle project detection and building
4. Follow interactive prompts for result management

## Best Practices

- Use basic profiling first, then comprehensive analysis
- Save important profiles with timestamp backups
- Compare results across different builds
- Use appropriate tool for specific analysis needs

For detailed help on any script, run it with --help option.
EOF
}

# -----------------------------------------------------------------------------
# Validation and Setup
# -----------------------------------------------------------------------------

##
# Validates that help utilities are properly configured.
#
# Usage:
#   validate_help_system
##
validate_help_system() {
    # Check if we can generate basic help components
    local test_header
    test_header=$(generate_help_header "test.sh" "Test script" 2>/dev/null)
    
    if [[ -z "$test_header" ]]; then
        handle_error "Help system validation failed."
    fi
    
    log_info "Help system validated successfully."
}
