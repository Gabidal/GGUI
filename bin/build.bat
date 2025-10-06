@echo off

rem ----------------------------------------------------------------------------
rem Initializes and builds the GGUI project locally.
rem - robust project root detection (git or directory layout fallback)
rem - checks for Meson and a C++ compiler, prints brief versions
rem - configures and compiles using Meson
rem ----------------------------------------------------------------------------

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
    echo Error: Unable to determine the project root directory. Ensure you're in the GGUI project directory.
    exit /b 1
)

rem Check if the current directory is the project root
for %%i in ("%project_root%") do set "project_root_name_only=%%~nxi"
if /i "%project_root_name_only%"=="%project_root_name%" (
    if /i "%current_dir%"=="%project_root%" (
        echo Info: In the project root directory. Changing to 'bin' directory.
        cd /d "%project_root%\%bin_dir_name%" || (
            echo Error: Failed to change to the 'bin' directory.
            exit /b 1
        )
    ) else (
        echo Info: Changing to the 'bin' directory within the project.
        cd /d "%project_root%\%bin_dir_name%" || (
            echo Error: Failed to change to the 'bin' directory.
            exit /b 1
        )
    )
)

goto :Continue
endlocal

rem Call the check_directory function to ensure we're in the correct directory
call :check_directory

:Continue

rem Step 1: Check if meson is installed
where meson >nul 2>nul
if errorlevel 1 (
    echo Error: meson could not be found. Please install meson before running this script.
    exit /b 1
)

rem Step 2: Check if g++ is installed
where g++ >nul 2>nul
if errorlevel 1 (
    echo Error: g++ could not be found. Please install g++ before running this script.
    exit /b 1
)

rem Step 3: Ensure CXX environment variable is set
if "%CXX%"=="" (
    rem Set CXX to the value of 'g++' if not set
    set "CXX=g++"
    echo Info: CXX environment variable not set. Defaulting to 'g++'.
)

rem Step 4: Compile the project using Meson
echo Info: Compiling the project using Meson...
meson compile -C build build_native_archive || (
    echo Error: Compilation failed.
    exit /b 1
)

rem Step 5: Completion message
echo Info: Build process completed successfully!

