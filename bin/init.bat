@echo off
setlocal EnableExtensions

rem =============================================================================
rem GGUI Project Initialization Script (Windows)
rem =============================================================================
rem This script initializes and builds the GGUI project locally. It validates
rem the environment, checks for required tools, manages the build setup, and
rem ensures analytics tools are properly configured.
rem
rem Features:
rem - Environment and requirements validation
rem - Automated build directory setup
rem - Project compilation with meson
rem - Analytics tools validation and setup
rem - Proper file permissions configuration
rem
rem =============================================================================

echo === GGUI Project Initialization ===
echo.

rem Step 0: Detect project root and cd into bin
for /f "delims=" %%i in ('git rev-parse --show-toplevel 2^>nul') do set "project_root=%%i"
if "%project_root%"=="" (
    echo Error: Unable to determine the project root directory. Ensure you're in the GGUI project.
    exit /b 1
)
cd /d "%project_root%\bin" || (
    echo Error: Failed to change to 'bin' directory.
    exit /b 1
)

rem Helper: check if a command exists in PATH
set "_missingTool="
where meson >nul 2>nul || set "_missingTool=meson"
where g++   >nul 2>nul || set "_missingTool=g++"

rem Also detect bash for analytics validation (Git Bash or similar)
where bash  >nul 2>nul && set "HAS_BASH=1"

echo Step 1: Validating environment and analytics tools...
if exist ".\analytics\utils\validate.sh" (
    if defined HAS_BASH (
        rem Normalize line endings to avoid $'\r' issues in Git Bash
        bash -lc "sed -i 's/\r$//' ./analytics/utils/validate.sh 2>/dev/null || true; sed -i 's/\r$//' ./analytics/*.sh 2>/dev/null || true; sed -i 's/\r$//' ./analytics/utils/*.sh 2>/dev/null || true; sed -i 's/\r$//' ./build.sh 2>/dev/null || true" >nul 2>nul
        bash "./analytics/utils/validate.sh"
    ) else (
        echo Warning: 'bash' not found. Skipping analytics validation.
        if defined _missingTool (
            echo Error: Required tool not found: %_missingTool%
            exit /b 1
        )
    )
) else (
    echo Warning: Analytics validation script not found. Continuing with basic checks...
    if defined _missingTool (
        echo Error: Required tool not found: %_missingTool%
        exit /b 1
    )
)

echo.
echo Step 2: Setting up build environment...
if "%CXX%"=="" (
    set "CXX=g++"
    echo CXX environment variable was not set. Defaulting to 'g++'.
)

rem Check if ./build, ./build-win, ./build-release, ./build-linux exists, if so then remove them
echo Checking for old build directories...
if exist "build" (
    echo Removing old 'build' directory...
    rmdir /s /q "build"
    if errorlevel 1 (
        echo Error: Failed to remove old 'build' directory.
        exit /b 1
    )
)
if exist "build-release" (
    echo Removing old 'build-release' directory...
    rmdir /s /q "build-release"
    if errorlevel 1 (
        echo Error: Failed to remove old 'build-release' directory. 
        exit /b 1
    )
)
if exist "build-win" (
    echo Removing old 'build-win' directory...
    rmdir /s /q "build-win"
    if errorlevel 1 (
        echo Error: Failed to remove old 'build-win' directory.
        exit /b 1
    )
)
if exist "build-linux" (
    echo Removing old 'build-linux' directory...
    rmdir /s /q "build-linux"
    if errorlevel 1 (
        echo Error: Failed to remove old 'build-linux' directory.
        exit /b 1
    )
)

echo Step 3: Setting up the debug build directory...
rem Since the build directories are already removed, we can just assume build dir is gone.
meson setup build -Dbuildtype=debug
if errorlevel 1 (
    echo Error: Failed to configure the build directory.
    exit /b 1
)

echo Step 4: Compiling basic debug build...
meson compile -C build
if errorlevel 1 (
    echo Error: Compilation failed while building GGUI.
    exit /b 1
)

echo === Initialization Complete ===
echo Run ./bin/test.sh to test GGUI integrity
echo Run ./bin/export.sh to export GGUI as linkable libraries and auto generate headers
echo Run ./bin/analytics/* scripts for performance and memory leak checks

endlocal
