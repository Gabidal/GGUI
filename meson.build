project(
  'GGUI', 'cpp',
  version : '0.1.1',
  # default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
  default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=false', 'buildtype=release', 'optimization=3']
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

cpp_args = ['-fpermissive', '-Wno-narrowing']

executable(
  'GGUI',
  sources,
  install : true,
  cpp_args: cpp_args
)

