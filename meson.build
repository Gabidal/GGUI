project(
  'GGUI', 'cpp',
  version : '0.1.1',
  default_options : ['warning_level=3', 'cpp_std=c++17']
)

incdir = include_directories('C:/Users/Quanf/source/repos/VULKAN/Include/')

cpp = meson.get_compiler('cpp')

vulkanlib = cpp.find_library('C:/Users/Quanf/source/repos/VULKAN/lib/vulkan-1')

sources = [

  'Vulkan.cpp',

  'main.cpp',

]

executable(
  'GGUI',
  sources,
  install : true,
  include_directories : incdir,
  dependencies : vulkanlib
)

