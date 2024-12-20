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
  # Default build options
  default_options : [
    'warning_level=3',           # Set warning level to 3 (most verbose)
    'cpp_std=c++17',             # Use C++17 standard
    'debug=true',                # Enable debugging (set to false for release builds)
    'buildtype=debug'            # Default build type (can be changed to release or profile)
  ]
)

# Build type options
DEBUG = 1
RELEASE = 2
PROFILE = 3
TYPE = DEBUG  # Change this to RELEASE or PROFILE for different build types

# Source files for the project
sources = [
  # Core utilities
  '../Core/Utils/Color.cpp',
  '../Core/Utils/Style.cpp',
  '../Core/Utils/Logger.cpp',
  
  # Elements
  '../Elements/Element.cpp',
  '../Elements/Window.cpp',
  '../Elements/List_View.cpp',
  '../Elements/Text_Field.cpp',
  '../Elements/Button.cpp', 
  '../Elements/Switch.cpp',
  '../Elements/Canvas.cpp',
  '../Elements/Progress_Bar.cpp',

  # Specialized elements
  '../Elements/File_Streamer.cpp',
  '../Elements/HTML.cpp',

  # Core functionalities
  '../Core/Utils/No_Inline.cpp',
  '../Core/Settings.cpp',
  '../Core/Renderer.cpp',
  '../Core/main.cpp'
]

# SIMD and CPU-specific options
SSE     = '-msse'
AVX     = '-mavx'
AVX512  = '-mavx512f'

# Default SIMD mode (can be adjusted for specific requirements)
SIMD_MODE = AVX

# Common C++ compiler flags
cpp_args = [
  '-fpermissive',               # Allow some non-standard C++ constructs
  '-Wno-narrowing'              # Disable narrowing warnings (e.g., casting to a smaller type)
]

# Platform-specific settings
# Check if we're on an Android environment
if host_machine.system() == 'android'
  message('Android environment detected: Disabling SIMD flags')
  # Disable SIMD flags on Android due to potential compatibility issues
else
  cpp_args += [SIMD_MODE]
endif

# Linker arguments (e.g., libraries)
link_args = []

# Profiling setup (only enabled for the PROFILE build type)
if TYPE == PROFILE
  cpp_args += ['-pg']           # Enable profiling flags for GCC
  link_args += ['-pg']          # Link with profiling support
endif

# Windows-specific settings (e.g., for debugging)
if host_machine.system() == 'windows'
  link_args += ['-lDbgHelp']    # Link with DbgHelp library for better debugging support
endif

# Define the executable to be built
executable(
  'GGUI',                      # Output executable name
  sources,                     # Source files
  install : true,               # Install the executable after building
  cpp_args: cpp_args,          # C++ compiler flags
  link_args: link_args         # Linker flags
)
