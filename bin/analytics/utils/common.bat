
rem =============================================================================
rem Common Utility Functions (Windows equivalents of common.sh)
rem =============================================================================

rem ##
rem # Handles errors and exits gracefully with an error message.
rem #
rem # Arguments:
rem #   %~1 - Error message to display
rem ##
:handle_error
echo Error: %~1 1>&2
exit /b 1

rem ##
rem # Logs informational messages with a consistent format.
rem #
rem # Arguments:
rem #   %~1 - Message to log
rem ##
:log_info
echo [INFO] %~1
exit /b 0

rem ##
rem # Logs warning messages with a consistent format.
rem #
rem # Arguments:
rem #   %~1 - Warning message to log
rem ##
:log_warning
echo [WARNING] %~1 1>&2
exit /b 0

rem ##
rem # Returns project root directory and changes directory into there.
rem # Sets PROJECT_ROOT variable.
rem ##
:go_to_project_root
for /f "delims=" %%i in ('git rev-parse --show-toplevel 2^>nul') do set "PROJECT_ROOT=%%i"
if "%PROJECT_ROOT%"=="" (
    call :handle_error "Unable to determine the project root directory."
    exit /b 1
)
cd /d "%PROJECT_ROOT%" || (
    call :handle_error "Failed to change to project root directory."
    exit /b 1
)
exit /b 0

rem ##
rem # Returns the Meson build directory for a given build type.
rem # Sets BUILD_DIR variable.
rem #
rem # Arguments:
rem #   %~1 - build type: "debug" (default) or "release" or "profile"
rem #   %~2 - platform-specific (win, linux) [optional]
rem ##
:get_build_dir_for_type
set "build_type=%~1"
set "platform_type=%~2"
if "%build_type%"=="" set "build_type=debug"

set "BUILD_DIR=%SCRIPT_DIR%\build"
if "%build_type%"=="release" set "BUILD_DIR=%SCRIPT_DIR%\build-release"
if "%build_type%"=="profile" set "BUILD_DIR=%SCRIPT_DIR%\build-profile"

if not "%platform_type%"=="" set "BUILD_DIR=%BUILD_DIR%-%platform_type%"
exit /b 0

rem ##
rem # Remaps build type to meson buildtype string.
rem # Sets MESON_BUILD_TYPE variable.
rem #
rem # Arguments:
rem #   %~1 - build type (debug, release, profile)
rem ##
:meson_remap_build_type
set "remap_type=%~1"
if "%remap_type%"=="" set "remap_type=debug"

if "%remap_type%"=="debug" (
    set "MESON_BUILD_TYPE=debug"
) else if "%remap_type%"=="release" (
    set "MESON_BUILD_TYPE=release"
) else if "%remap_type%"=="profile" (
    set "MESON_BUILD_TYPE=debugoptimized"
) else (
    call :handle_error "Unknown build type: %remap_type%"
    exit /b 1
)
exit /b 0

rem ##
rem # Ensures a Meson build directory is configured (setup or reconfigure).
rem #
rem # Arguments:
rem #   %~1 - build type (profile, release, debug)
rem #   %~2 - cross platform type (optional)
rem #   %~3 - cross.ini file (if %~2 is given)
rem ##
:meson_setup_or_reconfigure
set "setup_build_type=%~1"
set "setup_platform_type=%~2"
set "setup_platform_ini=%~3"

call :get_build_dir_for_type "%setup_build_type%" "%setup_platform_type%"
call :meson_remap_build_type "%setup_build_type%"

set "meson_build_location=%SCRIPT_DIR%"
set "cross_file_opt="
if not "%setup_platform_ini%"=="" set "cross_file_opt=--cross-file %setup_platform_ini%"

if exist "%BUILD_DIR%" (
    call :log_info "Reconfiguring existing build directory: %BUILD_DIR%"
    meson setup --reconfigure %cross_file_opt% "%BUILD_DIR%" "%meson_build_location%" -Dbuildtype=%MESON_BUILD_TYPE%
    if errorlevel 1 (
        call :log_warning "Reconfiguration failed, removing directory for clean slate..."
        rmdir /s /q "%BUILD_DIR%" 2>nul
    )
)

if not exist "%BUILD_DIR%" (
    call :log_info "Configuring Meson build directory: %BUILD_DIR%"
    meson setup %cross_file_opt% "%BUILD_DIR%" "%meson_build_location%" -Dbuildtype=%MESON_BUILD_TYPE%
    if errorlevel 1 (
        call :handle_error "Meson setup failed for %BUILD_DIR%"
        exit /b 1
    )
)
exit /b 0

rem ##
rem # Compiles the Meson build for the requested type.
rem #
rem # Arguments:
rem #   %~1 - build type: "debug" (default) or "release" or "profile"
rem #   %~2 - target (defaults to build_native_archive)
rem #   %~3 - platform type (optional)
rem ##
:meson_compile_target
set "compile_build_type=%~1"
set "compile_targets=%~2"
set "compile_platform_type=%~3"
if "%compile_build_type%"=="" set "compile_build_type=debug"
if "%compile_targets%"=="" set "compile_targets=build_native_archive"

call :get_build_dir_for_type "%compile_build_type%" "%compile_platform_type%"

call :log_info "Compiling %compile_build_type% build at %BUILD_DIR%"
meson compile -C "%BUILD_DIR%" %compile_targets%
if errorlevel 1 (
    call :handle_error "Build failed for %compile_build_type% (%BUILD_DIR%)"
    exit /b 1
)
exit /b 0

rem ##
rem # Removes all existing build directories (build, build-*, etc.)
rem ##
:remove_build_directories
for /d %%d in ("%SCRIPT_DIR%\build*") do (
    if exist "%%d" (
        echo Removing existing build directory: %%d
        rmdir /s /q "%%d"
        if errorlevel 1 (
            call :handle_error "Failed to remove directory: %%d"
            exit /b 1
        )
    )
)
exit /b 0

rem ##
rem # Runs analytics validation via bash if available, otherwise does basic checks.
rem ##
:run_analytics_validation
where bash >nul 2>nul
if errorlevel 1 (
    call :log_warning "'bash' not found. Performing basic tool validation..."
    call :validate_basic_tools
    exit /b %errorlevel%
)

rem Bash is available - normalize line endings and run validation script
if exist "%SCRIPT_DIR%\analytics\utils\validate.sh" (
    bash -lc "sed -i 's/\r$//' '%SCRIPT_DIR%/analytics/utils/validate.sh' 2>/dev/null || true; sed -i 's/\r$//' '%SCRIPT_DIR%/analytics'/*.sh 2>/dev/null || true; sed -i 's/\r$//' '%SCRIPT_DIR%/analytics/utils'/*.sh 2>/dev/null || true; sed -i 's/\r$//' '%SCRIPT_DIR%'/*.sh 2>/dev/null || true" >nul 2>nul
    bash "%SCRIPT_DIR%\analytics\utils\validate.sh"
    exit /b %errorlevel%
) else (
    call :log_warning "Analytics validation script not found. Continuing with basic checks..."
    call :validate_basic_tools
    exit /b %errorlevel%
)

rem ##
rem # Validates that basic required tools are available.
rem ##
:validate_basic_tools
set "missing_tools="
where meson >nul 2>nul || set "missing_tools=meson"
where g++ >nul 2>nul || set "missing_tools=%missing_tools% g++"

if not "%missing_tools%"=="" (
    call :handle_error "Missing required tools:%missing_tools%. Please install them before continuing."
    exit /b 1
)
echo Environment validation completed successfully.
exit /b 0

rem ##
rem # Prompts user with a yes/no question and returns the result.
rem #
rem # Arguments:
rem #   %~1 - Question to ask the user
rem #   %~2 - (Optional) Default answer (y/n). Defaults to 'n'
rem #
rem # Returns:
rem #   0 for yes, 1 for no
rem ##
:prompt_yes_no
set "question=%~1"
set "default=%~2"
if "%default%"=="" set "default=n"

if /i "%default%"=="y" (
    set "prompt_suffix=[Y/n]"
) else (
    set "prompt_suffix=[y/N]"
)

set /p "answer=%question% %prompt_suffix% "

if "%answer%"=="" set "answer=%default%"

if /i "%answer%"=="y" exit /b 0
if /i "%answer%"=="Y" exit /b 0
exit /b 1
