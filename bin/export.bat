@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem GGUI Project Export Script (Windows)
rem =============================================================================
rem This script initializes the GGUI project with release options enabled.
rem Builds it for native platform ONLY!
rem =============================================================================

rem Store the script directory
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

rem Set path to common utility functions
set "COMMON_BAT=%SCRIPT_DIR%\analytics\utils\common.bat"
if not exist "%COMMON_BAT%" (
    echo Error: Could not find common.bat at %COMMON_BAT% 1>&2
    exit /b 1
)

rem Initialize project root
call "%COMMON_BAT%" go_to_project_root
if errorlevel 1 exit /b 1

set "EXPORT_DIR=%PROJECT_ROOT%\bin\export"

call "%COMMON_BAT%" meson_setup_or_reconfigure "release"
if errorlevel 1 exit /b 1

echo running tests...
call "%COMMON_BAT%" get_build_dir_for_type "release"
meson test -C "%BUILD_DIR%" -v --print-errorlogs
if errorlevel 1 (
    echo Error: meson tests failed; aborting export 1>&2
    exit /b 1
)

rem Export native artifacts (header + native static lib) via Meson run target from release build
call "%COMMON_BAT%" meson_compile_target "release" "build_native_archive"
if errorlevel 1 exit /b 1

endlocal
exit /b 0