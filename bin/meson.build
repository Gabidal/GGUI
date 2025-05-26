# -----------------------------------------------------------------------------
# GGUI Project Build Configuration
# -----------------------------------------------------------------------------
# This is the Meson build configuration for the GGUI project. It defines the
# project settings, source files, compilation flags, and platform-specific 
# configurations. The script supports different build types: Debug, Release, 
# and Profiling.
# -----------------------------------------------------------------------------

project(
  'GGUI', 'cpp',
  version : '0.1.8',
  #DEBUG
  default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
  #RELEASE
  #default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=false', 'buildtype=release', 'optimization=3']
  #PROFILE
  #default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug', 'b_ndebug=false', 'b_lto=false']
)

# Build type options
DEBUG = 1
RELEASE = 2
PROFILE = 3
TYPE = DEBUG  # Change this to RELEASE or PROFILE for different build types

# Source files for the project
sources = [
  # Core utilities
  '../src/core/utils/color.cpp',
  '../src/core/utils/style.cpp',
  '../src/core/utils/logger.cpp',
  '../src/core/utils/utils.cpp',
  '../src/core/utils/constants.cpp',
  
  # Elements
  '../src/elements/element.cpp',
  '../src/elements/window.cpp',
  '../src/elements/listView.cpp',
  '../src/elements/textField.cpp',
  '../src/elements/button.cpp', 
  '../src/elements/switch.cpp',
  '../src/elements/canvas.cpp',
  '../src/elements/progressBar.cpp',

  # Specialized elements
  '../src/elements/fileStreamer.cpp',
  '../src/elements/HTML.cpp',

  # core functionalities
  '../src/core/utils/noInline.cpp',
  '../src/core/settings.cpp',
  '../src/core/renderer.cpp',
  '../src/core/thread.cpp',
  '../src/core/addons/addons.cpp',
  '../src/main.cpp'
]

# SIMD and CPU-specific options
SSE     = '-msse'
AVX     = '-mavx'
AVX512  = '-mavx512f'

# Default SIMD mode (can be adjusted for specific requirements)
SIMD_MODE = ''

# Common C++ compiler flags
cpp_args = [
  '-fpermissive',                # Allow some non-standard C++ constructs
  '-Wno-narrowing'              # Disable narrowing warnings (e.g., casting to a smaller type)
  # '-fvisibility=hidden',         # Hide symbols by default
  # '-fPIC',                       # Generate position-independent code
  # '-fno-semantic-interposition', # Disable semantic interposition
]

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

# Linker arguments (e.g., libraries)
link_args = []

# Add custom flags based on build type
if TYPE == DEBUG
  cpp_args += ['-DGGUI_DEBUG', '-pg']
  link_args += ['-pg']
elif TYPE == RELEASE
  cpp_args += ['-DGGUI_RELEASE']
elif TYPE == PROFILE
  cpp_args += ['-DGGUI_PROFILE', '-DGGUI_DEBUG', '-pg']
  link_args += ['-pg']
endif

if TYPE == DEBUG or TYPE == PROFILE
  # Windows-specific settings (e.g., for debugging)
  if host_machine.system() == 'windows'
    link_args += ['-lDbgHelp', '-Wl,--export-all-symbols']    # Link with DbgHelp library for better debugging support
  elif host_machine.system() == 'linux'
    # Linux-specific settings can be added here if needed
    cpp_args += ['-rdynamic']  
  endif
endif

# Define the executable to be built
executable(
  'GGUI',                      # Output executable name
  sources,                     # Source files
  install : true,               # Install the executable after building
  cpp_args: cpp_args,          # C++ compiler flags
  link_args: link_args         # Linker flags
)
