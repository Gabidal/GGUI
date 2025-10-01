# -----------------------------------------------------------------------------
# GGUI Project Build Configuration
# -----------------------------------------------------------------------------
# This is the Meson build configuration for the GGUI project. It defines the
# project settings, source files, compilation flags, and platform-specific 
# configurations. The script supports different build types via Meson's
# standard `buildtype` option (set at meson setup time).
# -----------------------------------------------------------------------------

project(
  'GGUI', 'cpp',
  version : '0.1.8.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++17'
    # , 'b_sanitize=address,undefined',     # Enable this to check if there are any UB
  ]
)

# Source files for the project
lib_sources = [
  # Core utilities
  '../src/core/utils/style.cpp',
  '../src/core/utils/logger.cpp',
  '../src/core/utils/utils.cpp',
  '../src/core/utils/fileStreamer.cpp',
  '../src/core/utils/settings.cpp',
  '../src/core/utils/drm.cpp',
  
  # Elements
  '../src/elements/element.cpp',
  '../src/elements/listView.cpp',
  '../src/elements/textField.cpp',
  '../src/elements/switch.cpp',
  '../src/elements/canvas.cpp',
  '../src/elements/progressBar.cpp',

  # Specialized elements
  '../src/elements/HTML.cpp',

  # core functionalities
  '../src/core/renderer.cpp',
  '../src/core/thread.cpp',
  '../src/core/addons/addons.cpp',
]

# SIMD and CPU-specific options
NO_SIMD = ''
SSE     = '-msse'
AVX     = '-mavx'
AVX512  = '-mavx512f'

# Default SIMD mode (can be adjusted for specific requirements)
SIMD_MODE = NO_SIMD

# Compiler handle
cppc = meson.get_compiler('cpp')
is_cross = meson.is_cross_build()   # This is to stop infinite recursion, since cross-platform will start meson but with other host OS, which could trigger second cross-platform

# Use Meson's buildtype instead of a custom TYPE variable
build_type = get_option('buildtype')

# Common C++ warning flags (filtered per compiler)
base_warn_flags = [
  '-Wno-narrowing',              # Disable narrowing warnings
  '-Wshadow',
  '-Wdangling-reference',        # Not supported by some MinGW toolchains
  '-Wformat=2',
  '-Wnon-virtual-dtor',
  '-Wcast-align',
  '-Wunused',
  '-Wmisleading-indentation',
  '-Wduplicated-cond',
  '-Wduplicated-branches',
  '-Wlogical-op',
  '-Wnull-dereference',
  '-Wuseless-cast',
  '-Wdouble-promotion',
  '-Wimplicit-fallthrough',
]

# cross-compilers don't always have full argument coverage, so lets disable those that the cross compiler doesn't have.
cpp_args = []
foreach f : base_warn_flags
  if cppc.has_argument(f)
    cpp_args += [f]
  endif
endforeach

# Platform-specific settings
# Check if we're on an Android environment
if host_machine.system() == 'android'
  message('Android environment detected: Disabling SIMD flags')
  # Disable SIMD flags on Android due to potential compatibility issues
else
  if SIMD_MODE != ''
    cpp_args += [SIMD_MODE]  # Only add SIMD_MODE if it's not empty
  endif
endif

# Linker arguments (collected project-wide)
link_args = []

# Add custom flags based on Meson's buildtype
if build_type == 'debug'
  cpp_args += ['-DGGUI_DEBUG']  # no flto nor -Og since debugging becomes an hassle with weird line skipping.
elif build_type == 'release'
  cpp_args += ['-DGGUI_RELEASE']
  # Avoid -march=native for cross builds (e.g., MinGW)
  if not is_cross and cppc.has_argument('-march=native')
    cpp_args += ['-march=native']
  endif
  if cppc.has_argument('-flto')
    cpp_args += ['-flto']
    link_args += ['-flto']
  endif
  if cppc.has_argument('-fwhole-program')
    cpp_args += ['-fwhole-program']
    link_args += ['-fwhole-program']
  endif
endif

if build_type == 'debug' or build_type == 'profile'
  # Windows-specific settings (e.g., for debugging)
  if host_machine.system() == 'windows'
    link_args += ['-lDbgHelp', '-Wl,--export-all-symbols']    # Link with DbgHelp library for better debugging support
  elif host_machine.system() == 'linux'
    # Linux-specific settings can be added here if needed
    cpp_args += ['-rdynamic']  
  endif
endif

# Prefer static linkage for the runtime on Linux
if host_machine.system() == 'linux'
  # This will link libc, libstdc++, libgcc, and other deps statically if
  # corresponding .a archives are available. This is the most reliable way
  # to ensure libc and libstdc++ are not dynamically linked.
  # link_args += ['-static']
endif

# Apply computed flags project-wide so individual targets don't need to repeat them
if cpp_args.length() > 0
  add_project_arguments(cpp_args, language: 'cpp')
endif
if link_args.length() > 0
  add_project_link_arguments(link_args, language: 'cpp')
endif

# Build the core static library
ggui_core = static_library(
  'GGUIcore',
  lib_sources,
  install: false
)

# Build a non-thin archive from the core objects for external consumption
core_objs = ggui_core.extract_all_objects(recursive: true)

# Choose archive command depending on platform/compiler
archive_cmd = []
if host_machine.system() == 'windows' and cppc.get_id() == 'msvc'
  # MSVC: use lib.exe to create .lib
  archive_cmd = ['lib', '/OUT:../export/@OUTPUT@', '@INPUT@']
else
  # Unix-like / MinGW: use ar
  archive_cmd = ['ar', 'rcs', '../export/@OUTPUT@', '@INPUT@']
endif

build_native_archive = custom_target(
  'build_native_archive',
  input: core_objs,
  output: host_machine.system() == 'windows' ? 'GGUIWin.lib' : 'libGGUIUnix.a',
  command: archive_cmd,
  build_by_default: true
)

# Build the main demo executable linked with the core library
executable(
  'GGUI',
  '../src/main.cpp',
  link_with: ggui_core,
  install: true
)

# Header generator tool (reuse existing C++ file)
headergen = executable(
  'ggui_headergen',
  'export/buildGGUILib.cpp',
  install: false
)

# Generate amalgamated header into the build directory
generate_header = custom_target(
  'generate_header',
  output: 'GGUI.h',
  command: [
    headergen,
    '--headers-only',
    '--out', '../export/@OUTPUT@',
  '--source-root', '../../',
  ],
  build_by_default: true
)

# Only create export targets when not cross-compiling
# (avoids recursive calls when export scripts invoke meson with cross files)
if not is_cross
  if host_machine.system() == 'windows'
    # Cross-compile for Linux using a Meson cross file (requires a Linux cross toolchain on PATH)
    export_linux_script = files('export/meson_export_linux.bat')

    run_target(
      'export-linux',
      command: ['cmd', export_linux_script[0]],
      depends: []
    )
  else
    # Cross-compile for Windows using a Meson cross file (requires MinGW tools)
    export_win_script = files('export/meson_export_win.sh')

    run_target(
      'export-win',
      command: ['bash', export_win_script[0]],
      depends: []
    )
  endif
endif

# Test target — use exported artifacts to mirror original run.sh
tester_inc = include_directories('..', 'export')

# Test target — link with exported lib and include exported header
tester = executable(
  'tester',
  [
    '../test/tester.cpp',
    generate_header,
  ],
  include_directories: tester_inc,
  link_with: ggui_core,
  install: false
)

# Run the tester to see if there are any problems
test('GGUI unit tests', tester, depends: [generate_header, build_native_archive])

# Timing wrapper executables used by analytics/time.sh and time2.sh
timing_standing = executable(
  'timingStanding',
  [
    'analytics/timingWrappers/standing.cpp',
    generate_header,
  ],
  include_directories: tester_inc,
  link_with: ggui_core,
  install: false
)

timing_busy = executable(
  'timingBusy',
  [
    'analytics/timingWrappers/busy.cpp',
    generate_header,
  ],
  include_directories: tester_inc,
  link_with: ggui_core,
  install: false
)

# Compare two callgrind outputs and produce a filtered one (analytics/time2.cpp)
time2 = executable(
  'time2',
  [ 'analytics/time2.cpp' ],
  install: false
)
