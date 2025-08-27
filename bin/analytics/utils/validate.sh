#!/usr/bin/env bash

# =============================================================================
# GGUI Analytics Tools Validation Script
# =============================================================================
# This script validates the functionality of the refactored analytics tools
# by testing utility modules and verifying script integrity.
#
# Author: GGUI Analytics Team
# Version: 1.0
# =============================================================================

# Source utility modules for testing
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

# Test configuration
TEST_PASSED=0
TEST_FAILED=0
VERBOSE=${GGUI_TEST_VERBOSE:-false}

# =============================================================================
# Test Framework
# =============================================================================

test_function() {
    local test_name="$1"
    local test_command="$2"
    
    echo -n "Testing $test_name... "
    
    if $VERBOSE; then
        echo
        echo "Command: $test_command"
    fi
    
    if eval "$test_command" >/dev/null 2>&1; then
        echo "PASS"
        ((TEST_PASSED++))
        return 0
    else
        echo "FAIL"
        ((TEST_FAILED++))
        return 1
    fi
}

test_module() {
    local module_name="$1"
    local module_path="$SCRIPT_DIR/$module_name"
    
    echo "Testing module: $module_name"
    
    # Test module can be sourced
    if test_function "module loading" "source '$module_path' 2>/dev/null"; then
        # Module-specific tests
        case "$module_name" in
            "common.sh")
                test_common_module
                ;;
            "valgrind.sh")
                test_valgrind_module
                ;;
            "perf.sh")
                test_perf_module
                ;;
            "help.sh")
                test_help_module
                ;;
        esac
    fi
    echo
}

# =============================================================================
# Module-Specific Tests
# =============================================================================

test_common_module() {
    # Module should already be sourced from test_module
    test_function "error handling" "type handle_error"
    test_function "logging functions" "type log_info && type log_warning"
    test_function "directory management" "type ensure_bin_directory"
    test_function "user interaction" "type prompt_yes_no"
    test_function "validation functions" "type validate_tools"
    test_function "backup name generation" "generate_backup_name 'test.out' | grep -q '$(date +%Y)'"
}

test_valgrind_module() {
    # Module should already be sourced from test_module
    test_function "callgrind settings" "get_callgrind_minimal_settings | grep -q 'callgrind'"
    test_function "memcheck settings" "get_memcheck_minimal_settings | grep -q 'memcheck'"
    test_function "profile management" "type manage_profile_cleanup"
    test_function "valgrind validation" "type validate_valgrind_installation"
}

test_perf_module() {
    # Module should already be sourced from test_module
    test_function "perf events" "get_perf_basic_events | grep -q 'branch-misses'"
    test_function "perf options" "get_perf_basic_options | grep -q '\-g'"
    test_function "data management" "type manage_perf_data_cleanup"
    test_function "installation validation" "type validate_perf_installation"
}

test_help_module() {
    # Module should already be sourced from test_module
    test_function "help header generation" "generate_help_header 'test.sh' 'Test script' | grep -q 'Usage:'"
    test_function "help footer generation" "generate_help_footer | grep -q 'Environment:'"
    test_function "option parsing" "type parse_common_options"
    test_function "documentation generation" "type generate_analytics_guide"
}

# =============================================================================
# Script integrity Tests
# =============================================================================

test_script_integrity() {
    echo "Testing script integrity..."
    
    local scripts=("benchmark.sh" "benchmark2.sh" "leaks.sh" "time.sh")
    
    for script in "${scripts[@]}"; do
        local script_path="$SCRIPT_DIR/../$script"
        
        if [[ -f "$script_path" ]]; then
            test_function "$script help" "bash '$script_path' --help 2>/dev/null | grep -q 'Usage:'"
            test_function "$script syntax" "bash -n '$script_path'"
        else
            echo "Testing $script... FAIL (not found)"
            ((TEST_FAILED++))
        fi
    done
    echo
}

# =============================================================================
# Environment Tests
# =============================================================================

test_environment() {
    echo "Testing environment..."
    
    # Test basic shell requirements
    test_function "bash version" "[[ \${BASH_VERSION%%.*} -ge 4 ]]"
    test_function "git availability" "command -v git"
    test_function "awk availability" "command -v awk"
    test_function "bc availability" "command -v bc"
    test_function "timeout availability" "command -v timeout"
    
    # Test directory structure
    test_function "utils directory" "[[ -d '$SCRIPT_DIR' ]]"
    test_function "project structure" "[[ -f '$SCRIPT_DIR/../../build.sh' ]]"
    
    echo
}

# =============================================================================
# Tool Availability Tests
# =============================================================================

test_analytic_tool_availability() {
    echo "Testing analytic tool availability..."
    
    # Optional tools (don't fail if missing)
    local optional_tools=("valgrind" "kcachegrind")
    for tool in "${optional_tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            echo "Optional tool $tool: AVAILABLE"
        else
            echo "Optional tool $tool: NOT AVAILABLE"
        fi
    done
    
    # Specific path tools
    local path_tools=(
        "/usr/local/bin/perf:Linux perf"
        "/root/go/bin/pprof:pprof"
    )
    
    for tool_info in "${path_tools[@]}"; do
        local tool_path="${tool_info%%:*}"
        local tool_name="${tool_info##*:}"
        
        if [[ -x "$tool_path" ]]; then
            echo "Path tool $tool_name: AVAILABLE"
        else
            echo "Path tool $tool_name: NOT AVAILABLE"
        fi
    done
    
    echo
}

# =============================================================================
# Requirements Validation
# =============================================================================

test_requirements() {
    echo "Testing system requirements..."
    
    # Test required system packages
    local required_packages=("build-essential" "meson" "ninja-build")
    local missing_packages=()
    
    for package in "${required_packages[@]}"; do
        if dpkg -l | grep -q "^ii.*$package"; then
            echo "Required package $package: INSTALLED"
        else
            echo "Required package $package: MISSING"
            missing_packages+=("$package")
        fi
    done
    
    # Test compiler availability    
    if command -v g++ >/dev/null 2>&1; then
        local gpp_version_output=$(g++ --version | head -n1)
        echo "G++ compiler: AVAILABLE ($gpp_version_output)"
        
        # Check if g++ is actually clang in disguise
        if echo "$gpp_version_output" | grep -qi "clang"; then
            echo "ERROR: g++ command points to clang, GGUI relies on GCC specific C++ standard 17 constexpr tricks!"
            ((TEST_FAILED++))
        else
            # Extract and validate G++ version (must be 13 or higher)
            local gpp_version=$(echo "$gpp_version_output" | sed -n 's/.* \([0-9]\+\).*/\1/p')
            if [[ -n "$gpp_version" && "$gpp_version" -ge 13 ]]; then
                echo "G++ version: SUFFICIENT (version $gpp_version)"
            else
                echo "ERROR: G++ version $gpp_version is too old (minimum version 13 required)"
                ((TEST_FAILED++))
            fi
        fi
    else
        echo "G++ compiler: NOT AVAILABLE"
        missing_packages+=("g++")
    fi
    
    # Test pkg-config
    if command -v pkg-config >/dev/null 2>&1; then
        echo "pkg-config: AVAILABLE"
    else
        echo "pkg-config: NOT AVAILABLE"
        missing_packages+=("pkg-config")
    fi
    
    # Report missing packages
    if [ ${#missing_packages[@]} -gt 0 ]; then
        echo
        echo "Missing required packages: ${missing_packages[*]}"
        echo "Install with: sudo apt update && sudo apt install ${missing_packages[*]}"
        ((TEST_FAILED++))
    fi
    
    echo
}

test_build_system() {
    echo "Testing build system..."
    
    # Test meson availability and version
    if command -v meson >/dev/null 2>&1; then
        local meson_version=$(meson --version 2>/dev/null)
        echo "Meson: AVAILABLE (version $meson_version)"
        
        # Check minimum version (0.55.0 or higher recommended)
        if [[ -n "$meson_version" ]]; then
            local version_major=$(echo "$meson_version" | cut -d. -f1)
            local version_minor=$(echo "$meson_version" | cut -d. -f2)
            
            if (( version_major > 0 || (version_major == 0 && version_minor >= 55) )); then
                echo "Meson version: SUFFICIENT"
            else
                echo "Meson version: TOO OLD (minimum 0.55.0 recommended)"
                log_warning "Consider upgrading Meson for better compatibility"
            fi
        fi
    else
        echo "Meson: NOT AVAILABLE"
        ((TEST_FAILED++))
    fi
    
    # Test ninja availability
    if command -v ninja >/dev/null 2>&1; then
        echo "Ninja: AVAILABLE ($(ninja --version))"
    else
        echo "Ninja: NOT AVAILABLE"
        ((TEST_FAILED++))
    fi
    
    echo
}

test_project_structure() {
    echo "Testing GGUI project structure..."
    
    # Find project root
    local project_root
    project_root=$(git rev-parse --show-toplevel 2>/dev/null)
    
    if [[ -z "$project_root" ]]; then
        echo "Git repository: NOT FOUND"
        ((TEST_FAILED++))
        return 1
    fi
    
    echo "Git repository: FOUND at $project_root"
    
    # Test essential project files
    local essential_files=(
        "bin/meson.build:Main build file"
        "src/ggui.h:Main header"
        "bin/build.sh:Build script"
        "bin/init.sh:Init script"
    )
    
    for file_info in "${essential_files[@]}"; do
        local file_path="${file_info%%:*}"
        local file_desc="${file_info##*:}"
        local full_path="$project_root/$file_path"
        
        if [[ -f "$full_path" ]]; then
            echo "$file_desc: FOUND"
        else
            echo "$file_desc: MISSING ($file_path)"
            ((TEST_FAILED++))
        fi
    done
    
    # Test directory structure
    local essential_dirs=(
        "src:Source directory"
        "bin:Binary directory"
        "examples:Examples directory"
    )
    
    for dir_info in "${essential_dirs[@]}"; do
        local dir_path="${dir_info%%:*}"
        local dir_desc="${dir_info##*:}"
        local full_path="$project_root/$dir_path"
        
        if [[ -d "$full_path" ]]; then
            echo "$dir_desc: FOUND"
        else
            echo "$dir_desc: MISSING ($dir_path)"
            ((TEST_FAILED++))
        fi
    done
    
    echo
}

test_analytics_setup() {
    echo "Testing analytics setup..."
    
    # Test analytics directory structure
    local analytics_files=(
        "benchmark.sh:Valgrind profiling script"
        "benchmark2.sh:Perf profiling script"
        "leaks.sh:Memory analysis script"
        "time.sh:Performance growth script"
    )
    
    for file_info in "${analytics_files[@]}"; do
        local file_path="${file_info%%:*}"
        local file_desc="${file_info##*:}"
        local full_path="$SCRIPT_DIR/../$file_path"
        
        if [[ -f "$full_path" && -x "$full_path" ]]; then
            echo "$file_desc: READY"
        elif [[ -f "$full_path" ]]; then
            echo "$file_desc: FOUND but not executable"
            log_warning "Run: chmod +x $full_path"
            ((TEST_FAILED++))
        else
            echo "$file_desc: MISSING"
            ((TEST_FAILED++))
        fi
    done
    
    # Test utility modules
    local util_files=(
        "common.sh:Common utilities"
        "valgrind.sh:Valgrind utilities"
        "perf.sh:Perf utilities"
        "help.sh:Help utilities"
    )
    
    for file_info in "${util_files[@]}"; do
        local file_path="${file_info%%:*}"
        local file_desc="${file_info##*:}"
        local full_path="$SCRIPT_DIR/$file_path"
        
        if [[ -f "$full_path" ]]; then
            echo "$file_desc: READY"
        else
            echo "$file_desc: MISSING"
            ((TEST_FAILED++))
        fi
    done
    
    echo
}

# =============================================================================
# Main Execution
# =============================================================================

echo "GGUI Dependency Validation"
echo "==============================="
echo

# Test environment
test_environment

# Test system requirements
test_requirements

# Test build system
test_build_system

# Test project structure
test_project_structure

# Test tool availability
test_analytic_tool_availability

# Test analytics setup
test_analytics_setup

# Test utility modules
for module in "common.sh" "valgrind.sh" "perf.sh" "help.sh"; do
    test_module "$module"
done

# Test script integrity
test_script_integrity

# Summary
echo "Summary"
echo "=================="
echo "Tests passed: $TEST_PASSED"
echo "Tests failed: $TEST_FAILED"
echo "Total tests:  $((TEST_PASSED + TEST_FAILED))"
echo

if [[ $TEST_FAILED -eq 0 ]]; then
    echo "All tests passed! Ready to dev GGUI."
    echo
    echo "Next steps:"
    echo "- Run './bin/analytics/benchmark.sh --help' to see profiling options"
    echo "- Run './bin/analytics/leaks.sh' for basic memory analysis"
    echo "- See './bin/analytics/README.md' for comprehensive documentation"
    exit 0
else
    echo "Some tests failed. Please review the failures above."
    echo
    echo "Common solutions:"
    echo "- Install missing packages: sudo apt update && sudo apt install <packages>"
    echo "- Install missing tools (valgrind, perf, pprof)"
    echo "- Check file permissions: chmod +x bin/analytics/*.sh"
    echo "- Verify GGUI project structure and git repository"
    echo "- Ensure you're running from within the GGUI project"
    echo "- Missing gcc or too old version? Run: sudo apt install gcc-13 g++-13 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 60 --slave /usr/bin/g++ g++ /usr/bin/g++-13"
    exit 1
fi
