@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem Cross-compile Linux artifacts from a Windows host using a Linux cross toolchain.
rem Requires a working command prompt and a cross toolchain on PATH,
rem such as x86_64-linux-gnu-* from MSYS2 or a custom toolchain.
rem =============================================================================

rem Store the script directory
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

rem Source common utility functions
set "COMMON_BAT=%SCRIPT_DIR%\..\analytics\utils\common.bat"
if exist "%COMMON_BAT%" (
    call "%COMMON_BAT%"
) else (
    echo Error: Could not find common.bat at %COMMON_BAT% 1>&2
    exit /b 1
)

rem Initialize project root
call :go_to_project_root
if errorlevel 1 exit /b 1

set "CROSS_FILE=%PROJECT_ROOT%\bin\export\cross-linux.ini"

call :meson_setup_or_reconfigure "release" "linux" "%CROSS_FILE%"
if errorlevel 1 exit /b 1

call :meson_compile_target "release" "build_native_archive" "linux"
if errorlevel 1 exit /b 1

endlocal
exit /b 0
