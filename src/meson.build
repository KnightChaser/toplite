# src/meson.build
inc = include_directories('.', 'core', 'ui')

srcs = files('core/proc_iter.c', 'core/system.c', 'ui/render.c', 'main.c', 'util.c')

executable('toplite', srcs, include_directories: inc, install: true)
