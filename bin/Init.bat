@echo off
REM Initializes and builds GGUI locally.

REM Check if meson is installed or not
where meson >nul 2>nul
IF ERRORLEVEL 1 (
    echo "meson could not be found. Please install meson before running this script."
    exit /b
)

REM Check if g++ is installed or not
where g++ >nul 2>nul
IF ERRORLEVEL 1 (
    echo "g++ could not be found. Please install g++ before running this script."
    exit /b
)

REM Check if g++ is in CXX variable
IF "%CXX%"=="" (
    REM set CXX to value of 'g++'
    set CXX=g++
)

REM Check if there is ./Build folder
IF EXIST Build (
    meson setup --wipe Build
) ELSE (
    meson setup Build
)

REM Now compile
meson compile -C Build

echo "Done!"