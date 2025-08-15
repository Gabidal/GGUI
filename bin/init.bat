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
        if errorlevel 1 (
            echo Environment validation failed. Analytics tools may not work properly.
            set /p _CONT=Continue anyway? [y/N]:
            set "_CONT=%_CONT:~0,1%"
            if /i "%_CONT%"=="y" (
                rem continue
            ) else (
                echo Initialization cancelled.
                exit /b 1
            )
        ) else (
            echo Environment validation completed successfully.
        )
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

echo Step 3: Setting up the build directory...
if exist "build" (
    meson setup --wipe build -Dbuildtype=debug
    if errorlevel 1 (
        echo Error: Failed to reconfigure the build directory.
        exit /b 1
    )
) else (
    meson setup build -Dbuildtype=debug
    if errorlevel 1 (
        echo Error: Failed to configure the build directory.
        exit /b 1
    )
)

echo Step 4: Compiling the core application (GGUI)...
meson compile -C build GGUI
if errorlevel 1 (
    echo Error: Compilation failed while building GGUI.
    exit /b 1
)

echo Step 4.1: Preparing export artifacts (header and native library)...
set "HEADER_OUT=build\GGUI.h"
if not exist "%HEADER_OUT%" (
    if not exist "build\ggui_headergen.exe" (
        echo Building header generator...
        g++ -std=c++17 -O2 -o "build\ggui_headergen.exe" "export\buildGGUILib.cpp"
        if errorlevel 1 (
            echo Error: Failed to compile header generator.
            exit /b 1
        )
    )
    echo Generating amalgamated header...
    "build\ggui_headergen.exe" --headers-only --out "%HEADER_OUT%" --source-root ".." 
    if errorlevel 1 (
        echo Error: Header generation failed.
        exit /b 1
    )
)

rem Create export directory and copy artifacts
set "EXPORT_DIR=%cd%\export"
if not exist "%EXPORT_DIR%" (
    mkdir "%EXPORT_DIR%" >nul 2>nul
)
copy /Y "%HEADER_OUT%" "%EXPORT_DIR%\GGUI.h" >nul
if errorlevel 1 (
    echo Error: Failed to copy header to export directory.
    exit /b 1
)
rem Build non-thin native archive and copy to export
if exist "build\GGUIWin.lib" del /f /q "build\GGUIWin.lib" >nul 2>nul
if exist "build\libGGUIUnix.a" del /f /q "build\libGGUIUnix.a" >nul 2>nul
meson compile -C build build_native_archive
if errorlevel 1 (
    echo Error: Failed to build native archive.
    exit /b 1
)
if exist "build\GGUIWin.lib" (
    copy /Y "build\GGUIWin.lib" "%EXPORT_DIR%\GGUIWin.lib" >nul
) else if exist "build\libGGUIUnix.a" (
    copy /Y "build\libGGUIUnix.a" "%EXPORT_DIR%\libGGUIUnix.a" >nul
) else (
    echo Error: Native archive output not found after build.
    exit /b 1
)

rem Create stamp files newer than inputs to satisfy ninja deps
echo done> "build\exported_header.stamp"
echo done> "build\exported_native_lib.stamp"

echo Step 4.2: Compiling tester (uses exported artifacts)...
if exist "build\GGUIWin.lib" del /f /q "build\GGUIWin.lib" >nul 2>nul
meson compile -C build tester
if errorlevel 1 (
    echo Error: Tester build failed.
    exit /b 1
)

echo === Initialization Complete ===
echo Build process completed successfully!
echo GGUI executable: .\build\GGUI.exe

endlocal
