@echo off
rem Initializes and builds GGUI locally.

rem Function to check if the script is run from the project root or a subdirectory
:check_directory
setlocal
set "current_dir=%cd%"
set "project_root_name=GGUI"
set "bin_dir_name=bin"

rem Find the project root directory by looking for the .git directory
for /f "delims=" %%i in ('git rev-parse --show-toplevel 2^>nul') do set "project_root=%%i"

rem If git is not found or the project root is not determined
if "%project_root%"=="" (
    echo Unable to determine the project root directory. Make sure you're in the GGUI project.
    exit /b 1
)

rem Check if the current directory is the project root
for %%i in ("%project_root%") do set "project_root_name_only=%%~nxi"
if /i "%project_root_name_only%"=="%project_root_name%" (
    if /i "%current_dir%"=="%project_root%" (
        echo In the project root directory. Changing to 'bin' directory.
        cd /d "%project_root%\%bin_dir_name%"
    ) else (
        echo Changing to the 'bin' directory within the project.
        cd /d "%project_root%\%bin_dir_name%"
    )
)

goto :Continue
endlocal

rem Call the check_directory function to ensure we're in the correct directory
call :check_directory

:Continue

rem Check if meson is installed or not
where meson >nul 2>nul
if errorlevel 1 (
    echo meson could not be found. Please install meson before running this script.
    exit /b 1
)

rem Check if g++ is installed or not
where g++ >nul 2>nul
if errorlevel 1 (
    echo g++ could not be found. Please install g++ before running this script.
    exit /b 1
)

rem Check if g++ is in CXX variable
if "%CXX%"=="" (
    rem Set CXX to the value of 'g++'
    set "CXX=g++"
)

rem Now compile
meson compile -C Build

echo Done!
