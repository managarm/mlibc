libfoo = shared_library('foo', 'libfoo.c', link_args: mlibc_link_args,)
test_depends = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: mlibc_link_args,)
test_native_depends = [libfoo_native]
