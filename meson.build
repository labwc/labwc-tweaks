project(
  'labwc-tweaks',
  'c',
  version: '0.1.0',
  license: 'GPL-2.0-only',
  default_options: ['c_std=c11', 'warning_level=3'],
)

add_project_arguments(['-Wno-unused-parameter'], language: 'c',)

gtkdeps = [dependency('gtk+-3.0'), dependency('libxml-2.0'), dependency('gio-2.0')]

executable(
  meson.project_name(),
  files(
    'main.c',
    'xml.c',
    'theme.c',
    'keyboard-layouts.c',
    'stack-appearance.c',
    'stack-mouse.c',
    'update.c',
  ),
  dependencies: gtkdeps,
  install : true,
)

subdir('tests')
subdir('data')
