@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem GGUI Analytics Tools Validation Script (Windows)
rem =============================================================================
rem This script validates the functionality of the refactored analytics tools
rem by testing utility modules and verifying script integrity.
rem
rem Author: GGUI Analytics Team
rem Version: 1.0
rem =============================================================================
rem
rem Usage: call validate.bat <function_name> [args...]
rem Example: call validate.bat run_all_validations
rem Example: call validate.bat test_environment
rem =============================================================================

rem Store the script directory
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

rem Set path to common utility functions
set "COMMON_BAT=%SCRIPT_DIR%\common.bat"
if not exist "%COMMON_BAT%" (
    echo Error: Could not find common.bat at %COMMON_BAT% 1>&2
    exit /b 1
)

rem Test configuration
set "TEST_PASSED=0"
set "TEST_FAILED=0"
if "%GGUI_TEST_VERBOSE%"=="" set "VERBOSE=false"
if not "%GGUI_TEST_VERBOSE%"=="" set "VERBOSE=%GGUI_TEST_VERBOSE%"

rem ##
rem # If called with arguments, dispatch to the requested function
rem # On call: %~0 caller script, %~1 function label, %~2+ actual args
rem # Shift by one: %~0 function label (wrong but what can you do :/) %~1+ args
rem ##
if "%~1"=="" goto :run_all_validations
set "_fn=%~1"
shift /1
goto :%_fn%

rem =============================================================================
rem Test Framework
rem =============================================================================

rem ##
rem # Tests a single function/command and records pass/fail.
rem #
rem # Arguments:
rem #   %~1 - Test name
rem #   %~2 - Command to test (should return 0 for success)
rem ##
:test_function
set "test_name=%~1"
set "test_command=%~2"

echo|set /p="Testing %test_name%... "

if "%VERBOSE%"=="true" (
    echo.
    echo Command: %test_command%
)

%test_command% >nul 2>&1
if errorlevel 1 (
    echo FAIL
    set /a "TEST_FAILED+=1"
    exit /b 1
) else (
    echo PASS
    set /a "TEST_PASSED+=1"
    exit /b 0
)

rem =============================================================================
rem Environment Tests
rem =============================================================================

rem ##
rem # Tests the basic environment requirements.
rem ##
:test_environment
echo Testing environment...

rem Test basic shell requirements
call :test_function "git availability" "where git"
call :test_function "findstr availability" "where findstr"

echo.
exit /b 0

rem =============================================================================
rem Tool Availability Tests
rem =============================================================================

rem ##
rem # Tests availability of analytic tools.
rem ##
:test_analytic_tool_availability
echo Testing analytic tool availability...

rem Optional tools (don't fail if missing, just report)
set "optional_tools=valgrind kcachegrind perf"

for %%t in (%optional_tools%) do (
    where %%t >nul 2>&1
    if errorlevel 1 (
        echo tool %%t: NOT AVAILABLE ^(Optional^)
    ) else (
        echo tool %%t: AVAILABLE ^(Optional^)
    )
)

echo.
exit /b 0

rem =============================================================================
rem Requirements Validation
rem =============================================================================

rem ##
rem # Tests system requirements including required packages and compiler.
rem ##
:test_requirements
echo Testing system requirements...
set "missing_tools="

rem Test meson availability
where meson >nul 2>&1
if errorlevel 1 (
    echo Required tool meson: MISSING
    set "missing_tools=!missing_tools! meson"
) else (
    echo Required tool meson: INSTALLED
)

rem Test ninja availability
where ninja >nul 2>&1
if errorlevel 1 (
    echo Required tool ninja: MISSING
    set "missing_tools=!missing_tools! ninja"
) else (
    echo Required tool ninja: INSTALLED
)

rem Test compiler availability
where cl >nul 2>&1
if errorlevel 1 (
    where c++ >nul 2>&1
    if errorlevel 1 (
        where g++ >nul 2>&1
        if errorlevel 1 (
            where clang++ >nul 2>&1
            if errorlevel 1 (
                echo c++ compiler: NOT AVAILABLE
                set "missing_tools=!missing_tools! c++"
            ) else (
                echo c++ compiler clang++: AVAILABLE
            )
        ) else (
            echo c++ compiler g++: AVAILABLE
        )
    ) else (
        echo c++ compiler c++: AVAILABLE
    )
) else (
    echo c++ compiler cl ^(MSVC^): AVAILABLE
)

rem Test pkg-config (optional on Windows)
where pkg-config >nul 2>&1
if errorlevel 1 (
    echo pkg-config: NOT AVAILABLE ^(Optional on Windows^)
) else (
    echo pkg-config: AVAILABLE
)

rem Report missing tools
if not "!missing_tools!"=="" (
    echo.
    echo Missing required tools:!missing_tools!
    echo Please install them before continuing.
    set /a "TEST_FAILED+=1"
)

echo.
exit /b 0

rem =============================================================================
rem Build System Tests
rem =============================================================================

rem ##
rem # Tests the build system (meson/ninja) availability and version.
rem ##
:test_build_system
echo Testing build system...

rem Test meson availability and version
where meson >nul 2>&1
if errorlevel 1 (
    echo Meson: NOT AVAILABLE
    set /a "TEST_FAILED+=1"
) else (
    for /f "tokens=*" %%v in ('meson --version 2^>nul') do set "meson_version=%%v"
    echo Meson: AVAILABLE ^(version !meson_version!^)
    
    rem Parse version for comparison (simplified check)
    for /f "tokens=1,2 delims=." %%a in ("!meson_version!") do (
        set "version_major=%%a"
        set "version_minor=%%b"
    )
    
    rem Check minimum version (0.55.0 or higher recommended)
    if !version_major! GTR 0 (
        echo Meson version: SUFFICIENT
    ) else if !version_minor! GEQ 55 (
        echo Meson version: SUFFICIENT
    ) else (
        echo Meson version: TOO OLD ^(minimum 0.55.0 recommended^)
        call "%COMMON_BAT%" log_warning "Consider upgrading Meson for better compatibility"
    )
)

rem Test ninja availability
where ninja >nul 2>&1
if errorlevel 1 (
    echo Ninja: NOT AVAILABLE
    set /a "TEST_FAILED+=1"
) else (
    for /f "tokens=*" %%v in ('ninja --version 2^>nul') do set "ninja_version=%%v"
    echo Ninja: AVAILABLE ^(!ninja_version!^)
)

echo.
exit /b 0

rem =============================================================================
rem Analytics Setup Tests
rem =============================================================================

rem ##
rem # Tests the analytics setup and utility modules.
rem ##
:test_analytics_setup
echo No analytics support in windows yet, skipping...

echo.
exit /b 0

rem =============================================================================
rem Common Module Tests
rem =============================================================================

rem ##
rem # Tests the common.bat module functions.
rem ##
:test_common_module
echo Testing module: common.bat

rem Test module exists
if not exist "%COMMON_BAT%" (
    echo Testing module loading... FAIL
    set /a "TEST_FAILED+=1"
    exit /b 1
)

echo Testing module loading... PASS
set /a "TEST_PASSED+=1"

rem Test that common.bat can be called with various functions
call :test_function "error handling" "findstr /c:":handle_error" "%COMMON_BAT%""
call :test_function "logging functions" "findstr /c:":log_info" "%COMMON_BAT%""
call :test_function "directory management" "findstr /c:":go_to_project_root" "%COMMON_BAT%""
call :test_function "user interaction" "findstr /c:":prompt_yes_no" "%COMMON_BAT%""
call :test_function "validation functions" "findstr /c:":validate_basic_tools" "%COMMON_BAT%""
call :test_function "meson setup function" "findstr /c:":meson_setup_or_reconfigure" "%COMMON_BAT%""

echo.
exit /b 0

rem =============================================================================
rem Script Integrity Tests
rem =============================================================================

rem ##
rem # Tests script integrity by checking syntax and structure.
rem ##
:test_script_integrity
echo Testing script integrity...

rem Test batch scripts syntax by checking for common patterns
set "batch_scripts=..\..\..\bin\init.bat,..\..\..\bin\build.bat,..\..\..\bin\test.bat,..\..\..\bin\export.bat"

for %%s in (%batch_scripts%) do (
    set "script_path=%SCRIPT_DIR%\%%s"
    if exist "!script_path!" (
        rem Basic syntax check - ensure file contains expected batch patterns
        findstr /c:"@echo off" "!script_path!" >nul 2>&1
        if errorlevel 1 (
            echo Testing %%~nxs syntax... FAIL ^(missing @echo off^)
            set /a "TEST_FAILED+=1"
        ) else (
            echo Testing %%~nxs syntax... PASS
            set /a "TEST_PASSED+=1"
        )
    ) else (
        echo Testing %%~nxs... SKIP ^(not found^)
    )
)

echo.
exit /b 0

rem =============================================================================
rem Main Execution - Run All Validations
rem =============================================================================

:run_all_validations
echo GGUI Dependency Validation ^(Windows^)
echo ===============================
echo.

rem Test environment
call :test_environment

rem Test system requirements
call :test_requirements

rem Test build system
call :test_build_system

rem Test tool availability
call :test_analytic_tool_availability

rem Test analytics setup
call :test_analytics_setup

rem Test common module
call :test_common_module

rem Test script integrity
call :test_script_integrity

rem Summary
echo Summary
echo ==================
echo Tests passed: %TEST_PASSED%
echo Tests failed: %TEST_FAILED%
set /a "TOTAL_TESTS=%TEST_PASSED%+%TEST_FAILED%"
echo Total tests:  %TOTAL_TESTS%
echo.

if %TEST_FAILED% equ 0 (
    echo All tests passed! Ready to dev GGUI.
    endlocal
    exit /b 0
) else (
    echo Some tests failed. Please review the failures above.
    echo.
    echo Common solutions:
    echo - Install missing tools ^(meson, ninja, c++ compiler^)
    echo - Install Git for Windows with bash support
    echo - Ensure you're running from within the GGUI project
    echo - For full analytics support, use WSL or Git Bash
    endlocal
    exit /b 1
)

endlocal
exit /b 0
