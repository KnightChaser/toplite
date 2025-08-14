# src/meson.build
inc = include_directories('.', 'core', 'ui')

srcs = [
  'core/proc_iter.c',
  'core/process.c',
  'core/system.c',
  'core/sorting.c',
  'ui/render.c',
  'ui/terminal.c',
  'ui/input.c',
  'util/util.c',
  'main.c',
]

executable('toplite', srcs, include_directories: inc, install: true)
