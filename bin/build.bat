@echo off
setlocal EnableDelayedExpansion

rem ----------------------------------------------------------------------------
rem Expects project initialization by init.bat
rem Runs meson compile [target(s)] -C [build type]
rem
rem Use as:
rem build.bat profile ggui
rem build.bat debug time2 timingStanding timingBusy
rem ----------------------------------------------------------------------------

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
set "build_type=%~1"

rem Configure if needed
call "%COMMON_BAT%" meson_setup_or_reconfigure "%build_type%"
if errorlevel 1 exit /b 1

rem Build (pass remaining arguments as targets)
rem Shift past first argument and collect remaining args
set "targets="
shift
:collect_targets
if "%~1"=="" goto :done_targets
set "targets=%targets% %~1"
shift
goto :collect_targets

:done_targets
rem Trim leading space if any
if defined targets set "targets=%targets:~1%"

call "%COMMON_BAT%" meson_compile_target "%build_type%" "%targets%"
if errorlevel 1 exit /b 1

echo Build completed successfully

endlocal
exit /b 0

