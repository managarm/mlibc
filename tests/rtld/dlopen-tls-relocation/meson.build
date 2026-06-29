libfoo = shared_library('foo', 'libfoo.c', link_args: mlibc_link_args, dependencies: libc_dep)
test_depends = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
test_native_depends = [libfoo_native]
