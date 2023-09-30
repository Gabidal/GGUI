project(
  'GGUI', 'cpp',
  version : '0.1.1',
  default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=true', 'buildtype=debug']
  # default_options : ['warning_level=3', 'cpp_std=c++17', 'debug=false', 'buildtype=release', 'optimization=3']
)

sources = [
  'Elements/Button.cpp', 
  'Elements/Element.cpp',
  'Elements/List_View.cpp',
  'Elements/Switch.cpp',
  'Elements/Text_Field.cpp',
  'Elements/Window.cpp',
  'Elements/Canvas.cpp',
  'Elements/Progress_Bar.cpp',

  'Elements/File_Streamer.cpp',
  'Elements/HTML.cpp',

  'Renderer.cpp',

  'main.cpp',
]

executable(
  'GGUI',
  sources,
  install : true
)

