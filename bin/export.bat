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

rem Generate amalgamated header using headergen (build if missing)
set "HEADER_OUT=%BUILD_DIR_RELEASE%\GGUI.h"
set "SOURCE_ROOT=%SCRIPT_DIR%.."
if not exist "%BUILD_DIR_RELEASE%\ggui_headergen.exe" (
  echo %LOG_PREFIX% Building header generator...
  where g++ >nul 2>nul || (
    echo %LOG_PREFIX% ERROR: g++ not found to build header generator.
    popd >nul
    exit /b 1
  )
  g++ -std=c++17 -O2 -o "%BUILD_DIR_RELEASE%\ggui_headergen.exe" "%SCRIPT_DIR%export\buildGGUILib.cpp"
  if errorlevel 1 (
    echo %LOG_PREFIX% ERROR: Failed to compile header generator.
    popd >nul
    exit /b 1
  )
)
echo %LOG_PREFIX% Generating header...
"%BUILD_DIR_RELEASE%\ggui_headergen.exe" --headers-only --out "%HEADER_OUT%" --source-root "%SOURCE_ROOT%"
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Header generation failed.
  popd >nul
  exit /b 1
)
copy /Y "%HEADER_OUT" "%EXPORT_DIR%\GGUI.h" >nul

rem Build native archive and copy to export
meson compile -C "%BUILD_DIR_RELEASE%" build_native_archive
if errorlevel 1 (
  echo %LOG_PREFIX% ERROR: Failed to build native archive.
  popd >nul
  exit /b 1
)
if exist "%BUILD_DIR_RELEASE%\GGUIWin.lib" (
  copy /Y "%BUILD_DIR_RELEASE%\GGUIWin.lib" "%EXPORT_DIR%\GGUIWin.lib" >nul
) else if exist "%BUILD_DIR_RELEASE%\libGGUIUnix.a" (
  copy /Y "%BUILD_DIR_RELEASE%\libGGUIUnix.a" "%EXPORT_DIR%\libGGUIUnix.a" >nul
) else (
  echo %LOG_PREFIX% ERROR: Native archive output not found after build.
  popd >nul
  exit /b 1
)

rem 5) Attempt cross-exports if bash and scripts are available (optional)
where bash >nul 2>nul && (
  if exist "export\meson_export_win.sh" (
    echo %LOG_PREFIX% exporting Windows artifacts - cross-compile via bash script
  bash "export/meson_export_win.sh"
  if errorlevel 1 echo %LOG_PREFIX% Windows export failed - optional step
  ) else (
    echo %LOG_PREFIX% export/meson_export_win.sh not found; skipping Windows cross-export
  )
  if exist "export\meson_export_linux.sh" (
    echo %LOG_PREFIX% exporting Linux artifacts - cross-compile via bash script
  bash "export/meson_export_linux.sh"
  if errorlevel 1 echo %LOG_PREFIX% Linux export failed - optional step
  ) else (
    echo %LOG_PREFIX% export/meson_export_linux.sh not found; skipping Linux cross-export
  )
) || echo %LOG_PREFIX% 'bash' not found; skipping cross-exports

popd >nul
exit /b 0
