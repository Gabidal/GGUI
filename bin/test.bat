@echo off
setlocal EnableExtensions

rem =============================================================================
rem GGUI Test Runner (Windows)
rem Mirrors behavior of bin/test.sh:
rem - Configure or reconfigure Meson build directory (bin/build)
rem - Run meson tests with verbose output and error logs
rem - Forward any extra args to meson test
rem =============================================================================

set "SCRIPT_DIR=%~dp0"
set "BUILD_DIR=%SCRIPT_DIR%build"

echo [test] Starting test runner

pushd "%SCRIPT_DIR%" >nul || (
    echo [test] ERROR: Failed to change directory to script location.
    exit /b 1
)

where meson >nul 2>nul || (
    echo [test] ERROR: 'meson' not found in PATH. Please install Meson and ensure it's on PATH.
    popd >nul
    exit /b 1
)

if not exist "%BUILD_DIR%\" (
    echo [test] Configuring build directory: "%BUILD_DIR%"
    meson setup "%BUILD_DIR%"
    if errorlevel 1 (
        echo [test] ERROR: Meson setup failed.
        popd >nul
        exit /b 1
    )
 ) else (
    echo [test] Reconfiguring build directory: "%BUILD_DIR%"
    meson setup --reconfigure "%BUILD_DIR%"
    if errorlevel 1 (
        echo [test] ERROR: Meson reconfigure failed.
        popd >nul
        exit /b 1
    )
)

echo [test] Running test suite...
meson test -C "%BUILD_DIR%" -v --print-errorlogs %*
set "ERR=%ERRORLEVEL%"

popd >nul
exit /b %ERR%
