project(
  'labwc-tweaks',
  'c',
  version: '0.1.0',
  license: 'GPL-2.0-only',
  default_options: ['c_std=c11', 'warning_level=3'],
)

add_project_arguments(['-Wno-unused-parameter'], language: 'c',)

gtkdeps = [dependency('gtk+-3.0'), dependency('libxml-2.0')]

executable(
  meson.project_name(),
  files('main.c', 'xml.c', 'find-themes.c'),
  dependencies: gtkdeps,
  install : true,
)

subdir('data')
