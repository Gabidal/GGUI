project(
  'GGUI', 'cpp',
  version : '0.1.7',
  default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
  #default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=false', 'buildtype=release', 'optimization=3']
)

sources = [
  'Elements/Element.cpp',
  'Elements/Window.cpp',
  'Elements/List_View.cpp',
  'Elements/Text_Field.cpp',
  'Elements/Button.cpp', 
  'Elements/Switch.cpp',
  'Elements/Canvas.cpp',
  'Elements/Progress_Bar.cpp',

  'Elements/File_Streamer.cpp',
  'Elements/HTML.cpp',

  'Renderer.cpp',

  'main.cpp',
]

SSE     = '-msse'
AVX     = '-mavx'
AVX512  = '-mavx512f'

SIMD_MODE = AVX

cpp_args = [
  '-fpermissive', 
  '-Wno-narrowing', 
  SIMD_MODE
]
link_args = []

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

