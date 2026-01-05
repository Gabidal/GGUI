@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem GGUI Project Export Script (Windows)
rem =============================================================================
rem This script initializes the GGUI project with release options enabled.
rem Builds it for native and other platforms and moves the builded libraries into ./bin/export/*
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

rem Export Linux cross-compiled lib, if Linux cross-compiler is available (use release build)
rem Detect common Linux cross-compilers or allow user to provide CROSS_COMPILE_PREFIX env var
set "LINUX_PREFIX=%CROSS_COMPILE_PREFIX%"
if "%LINUX_PREFIX%"=="" (
    for %%p in (x86_64-linux-gnu- x86_64-unknown-linux-gnu- i686-linux-gnu-) do (
        where %%pc++ >nul 2>nul
        if not errorlevel 1 (
            set "LINUX_PREFIX=%%p"
            goto :found_linux_prefix
        )
    )
)
:found_linux_prefix

if not "%LINUX_PREFIX%"=="" (
    echo exporting Linux artifacts ^(cross-compile^) using prefix %LINUX_PREFIX% from release build
    call "%COMMON_BAT%" meson_compile_target "release" "export-linux"
    if errorlevel 1 exit /b 1
) else (
    echo Linux cross-compiler not found; skipping Linux export.
)

echo Exported artifacts are available in %EXPORT_DIR%\

endlocal
exit /b 0