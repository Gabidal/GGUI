@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem GGUI Project Initialization Script (Windows)
rem =============================================================================
rem This script initializes and builds the GGUI project locally. It validates
rem the environment, checks for required tools, manages the build setup, and
rem ensures analytics tools are configured.
rem =============================================================================

rem Store the script directory
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

rem =============================================================================
rem Source common utility functions (like 'source common.sh' in bash)
rem =============================================================================
set "COMMON_BAT=%SCRIPT_DIR%\analytics\utils\common.bat"
if exist "%COMMON_BAT%" (
    call "%COMMON_BAT%"
) else (
    echo Error: Could not find common.bat at %COMMON_BAT% 1>&2
    exit /b 1
)

rem Initialize project root
call :go_to_project_root
if errorlevel 1 exit /b 1

echo === GGUI Project Initialization ===
echo.

rem Step 1: Run analytics validation
echo Validating environment and analytics tools...
call :run_analytics_validation
set "validation_result=%errorlevel%"

if %validation_result% neq 0 (
    echo Environment validation failed. Please address the issues above.
    echo You can continue with basic build, but analytics tools may not work properly.
    
    rem Allow CI environments to bypass the prompt
    if defined CI (
        echo Force/CI mode detected ^(CI=%CI%^). Skipping question...
    ) else (
        call :prompt_yes_no "Continue regardless?" "n"
        if errorlevel 1 (
            echo Initialization aborted by user.
            exit /b 1
        )
    )
)
echo.

rem Step 2: Set up build environment
echo Setting up build environment...

rem Ensure the CXX variable is set (default to 'g++' if not)
if "%CXX%"=="" (
    set "CXX=g++"
    echo CXX environment variable was not set. Defaulting to 'g++'.
)

rem Remove all existing build directories matching build*
call :remove_build_directories
if errorlevel 1 exit /b 1

rem Step 3: Setting up the default build configure
echo Setting up the default build configure...
call :meson_setup_or_reconfigure "debug"
if errorlevel 1 exit /b 1

rem Step 4: Compile the project using meson
echo Compiling the project...
call :meson_compile_target "debug" "ggui_core"
if errorlevel 1 exit /b 1

echo.
echo === Initialization Complete ===
echo Run .\bin\test.bat to test GGUI integrity
echo Run .\bin\export.bat to export GGUI as linkable libraries and auto generate headers
echo Run .\bin\analytics\* scripts for performance and memory leak checks

endlocal
exit /b 0