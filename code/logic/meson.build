dir = include_directories('.')
cc = meson.get_compiler('c')

fossil_threads_lib = library('fossil_threads',
    files('thread.c', 'mutex.c', 'cond.c'),
    install: true,
    dependencies: [cc.find_library('m', required: false), dependency('threads')],
    include_directories: dir)

fossil_threads_dep = declare_dependency(
    link_with: [fossil_threads_lib],
    dependencies: [cc.find_library('m', required: false), dependency('threads')],
    include_directories: dir)

meson.override_dependency('fossil-threads', fossil_threads_dep)