@echo off
rem ----------------------------------------------------------------------------
rem Initializes and builds the GGUI project locally. Ensures the script is run
rem from the project root directory, checks for required tools (meson, g++),
rem manages the build setup and compilation process, and provides user feedback.
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
    echo Error: Unable to determine the project root directory. Ensure you're in the GGUI project.
    exit /b 1
)

rem Check if the current directory is the project root
for %%i in ("%project_root%") do set "project_root_name_only=%%~nxi"
if /i "%project_root_name_only%"=="%project_root_name%" (
    if /i "%current_dir%"=="%project_root%" (
        echo In the project root directory. Changing to 'bin' directory.
        cd /d "%project_root%\%bin_dir_name%" || (
            echo Error: Failed to change to 'bin' directory.
            exit /b 1
        )
    ) else (
        echo Changing to the 'bin' directory within the project.
        cd /d "%project_root%\%bin_dir_name%" || (
            echo Error: Failed to change to 'bin' directory.
            exit /b 1
        )
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
    echo Error: meson could not be found. Please install meson before running this script.
    exit /b 1
)

rem Check if g++ is installed or not
where g++ >nul 2>nul
if errorlevel 1 (
    echo Error: g++ could not be found. Please install g++ before running this script.
    exit /b 1
)

rem Ensure the CXX environment variable is set (default to 'g++' if not)
if "%CXX%"=="" (
    rem Set CXX to the value of 'g++'
    set "CXX=g++"
    echo CXX environment variable not set. Defaulting to 'g++'.
)

rem Step 1: Set up the Build directory (wipe existing if necessary)
echo Setting up the Build directory...
if exist "Build" (
    meson setup --wipe Build || (
        echo Error: Failed to set up the Build directory.
        exit /b 1
    )
) else (
    meson setup Build || (
        echo Error: Failed to set up the Build directory.
        exit /b 1
    )
)

rem Step 2: Compile the project using meson
echo Compiling the project...
meson compile -C Build || (
    echo Error: Compilation failed.
    exit /b 1
)

rem Completion message
echo Build process completed successfully!
