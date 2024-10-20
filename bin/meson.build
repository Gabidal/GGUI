project(
  'GGUI', 'cpp',
  version : '0.1.7.5',
  #DEBUG
  default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
  #RELEASE
  #default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=false', 'buildtype=release', 'optimization=3']
  #PROFILE
  #default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
)

DEBUG = 1
RELEASE = 2
PROFILE = 3
TYPE = DEBUG

sources = [
  '../Core/Utils/Style.cpp',

  '../Elements/Element.cpp',
  '../Elements/Window.cpp',
  '../Elements/List_View.cpp',
  '../Elements/Text_Field.cpp',
  '../Elements/Button.cpp', 
  '../Elements/Switch.cpp',
  '../Elements/Canvas.cpp',
  '../Elements/Progress_Bar.cpp',

  '../Elements/File_Streamer.cpp',
  '../Elements/HTML.cpp',

  '../Core/utils/No_Inline.cpp',
  '../Core/Renderer.cpp',

  '../Core/main.cpp',
]

SSE     = '-msse'
AVX     = '-mavx'
AVX512  = '-mavx512f'

SIMD_MODE = AVX

cpp_args = [
  '-fpermissive', 
  '-Wno-narrowing'
]

# Check if we're on an Android environment
if host_machine.system() == 'android'
  message('Android environment detected: Disabling SIMD flags')
else
  cpp_args += [SIMD_MODE]
endif

link_args = []

# Add profiling flag
if TYPE == PROFILE
  cpp_args += ['-pg']
  link_args += ['-pg']
endif

# Add Windows-specific compile arguments
if host_machine.system() == 'windows'
  link_args += ['-lDbgHelp']
endif

executable(
  'GGUI',
  sources,
  install : true,
  cpp_args: cpp_args,
  link_args: link_args
)

