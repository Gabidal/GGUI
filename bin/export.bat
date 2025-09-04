@echo off
setlocal EnableExtensions

rem =============================================================================
rem GGUI Export Orchestrator (Windows)
rem Mirrors behavior of bin/export.sh:
rem - Ensure native build exists and tests pass (bin/build)
rem - Prepare a release build (bin/build-release)
rem - Export native artifacts (header + platform lib) via Meson run target
rem - Attempt Linux cross export via export-linux if bat and script available
rem =============================================================================

set "SCRIPT_DIR=%~dp0"
set "BUILD_DIR=%SCRIPT_DIR%build"
set "BUILD_DIR_RELEASE=%SCRIPT_DIR%build-release"
set "EXPORT_DIR=%SCRIPT_DIR%export"

set "LOG_PREFIX=[export]"

echo %LOG_PREFIX% starting export

pushd "%SCRIPT_DIR%" >nul || (
  echo %LOG_PREFIX% ERROR: Failed to change directory to script location.
  exit /b 1
)

where meson >nul 2>nul || (
  echo %LOG_PREFIX% ERROR: 'meson' not found in PATH. Please install Meson and ensure it's on PATH.
  popd >nul
  exit /b 1
)

rem 1) Configure native build directory (debug by default)
echo %LOG_PREFIX% configuring native build at "%BUILD_DIR%"
if not exist "%BUILD_DIR%\" (
  meson setup "%BUILD_DIR%"
) else (
  meson setup --reconfigure "%BUILD_DIR%"
)
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Meson setup/reconfigure failed for native build.
  popd >nul
  exit /b 1
)

rem 2) Run tests (verbose + error logs)
echo %LOG_PREFIX% running tests
meson test -C "%BUILD_DIR%" -v --print-errorlogs
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Tests failed; aborting export.
  popd >nul
  exit /b 1
)

rem 3) Prepare dedicated release build directory
echo %LOG_PREFIX% configuring release build at "%BUILD_DIR_RELEASE%" (buildtype=release)
if not exist "%BUILD_DIR_RELEASE%\" (
  meson setup "%BUILD_DIR_RELEASE%" -Dbuildtype=release
) else (
  rem Try meson configure to set buildtype; fall back to reconfigure when needed
  meson configure "%BUILD_DIR_RELEASE%" -Dbuildtype=release >nul 2>nul
  if errorlevel 1 (
    meson setup --reconfigure "%BUILD_DIR_RELEASE%" -Dbuildtype=release
  )
)
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Failed to configure release build.
  popd >nul
  exit /b 1
)

rem 4) Export native artifacts from release build (manual path for reliability on Windows)
echo %LOG_PREFIX% exporting native artifacts (manual)
if not exist "%EXPORT_DIR%\" mkdir "%EXPORT_DIR%" >nul 2>nul

rem 5) Build native archive
meson compile -C "%BUILD_DIR_RELEASE%" build_native_archive
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Failed to build native archive.
  popd >nul
  exit /b 1
)

rem 6) Cross-compile for linux as well.
meson compile -C "%BUILD_DIR_RELEASE%" export-linux

popd >nul
exit /b 0
