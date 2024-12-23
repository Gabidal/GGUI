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
TYPE = PROFILE  # Change this to RELEASE or PROFILE for different build types

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
SIMD_MODE = ''

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
  if SIMD_MODE != ''
    cpp_args += [SIMD_MODE]  # Only add SIMD_MODE if it's not empty
  endif
endif

# Linker arguments (e.g., libraries)
link_args = []

# Profiling setup (only enabled for the PROFILE build type)
if TYPE == PROFILE
  cpp_args += ['-pg','-rdynamic']           # Enable profiling flags for GCC
  link_args += ['-pg']                      # Link with profiling support
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
