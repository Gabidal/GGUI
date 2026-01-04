@echo off
setlocal EnableDelayedExpansion

rem =============================================================================
rem GGUI Test Runner (Windows)
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

rem Get build type from first argument
set "BUILD_TYPE=%~1"

call "%COMMON_BAT%" meson_setup_or_reconfigure "%BUILD_TYPE%"
if errorlevel 1 exit /b 1

rem Run tests (verbose + print error logs). Extra args are forwarded to meson test.
rem Shift past first argument and collect remaining args for meson test
set "extra_args="
shift
:collect_test_args
if "%~1"=="" goto :done_test_args
set "extra_args=%extra_args% %~1"
shift
goto :collect_test_args

:done_test_args
call "%COMMON_BAT%" get_build_dir_for_type "%BUILD_TYPE%"
meson test -C "%BUILD_DIR%" -v --print-errorlogs %extra_args%
set "ERR=%ERRORLEVEL%"

endlocal
exit /b %ERR%
