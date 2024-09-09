dir = include_directories('.')

code_deps = [
    dependency('threads'),
    meson.get_compiler('c').find_library('m', required : false)
]

fossil_threads_lib = library('fossil-threads',
    files('fiber.c', 'threads.c', 'pool.c', 'sync.c'),
    dependencies : [code_deps],
    install: EXIT_SUCCESS,
    include_directories: dir)

fossil_threads_dep = declare_dependency(
    link_with: [fossil_threads_lib],
    dependencies : [code_deps],
    include_directories: dir)
