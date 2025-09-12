@echo off
setlocal enabledelayedexpansion

REM Cross-compile Linux artifacts from a Windows host using a Linux cross toolchain.
REM Requires a working command prompt and a cross toolchain on PATH,
REM such as x86_64-linux-gnu-* from MSYS2 or a custom toolchain.

set "ROOT_DIR=%~dp0..\..\"
set "BUILD_LINUX=%ROOT_DIR%bin\build-linux"
set "CROSS_FILE=%ROOT_DIR%bin\export\cross-linux.ini"

mkdir "%BUILD_LINUX%" 2>nul

REM Setup or reconfigure cross build targeting Linux
if exist "%BUILD_LINUX%\build.ninja" (
    meson setup --reconfigure --cross-file "%CROSS_FILE%" "%BUILD_LINUX%" "%ROOT_DIR%bin"
) else (
    meson setup --cross-file "%CROSS_FILE%" "%BUILD_LINUX%" "%ROOT_DIR%bin"
)

REM Build only the core library to avoid running any target executables
meson compile -C "%BUILD_LINUX%" GGUIcore
if %errorlevel% neq 0 (
    echo Cross compile (Linux) failed. Check %BUILD_LINUX%\meson-logs\meson-log.txt >&2
    exit /b 1
)
