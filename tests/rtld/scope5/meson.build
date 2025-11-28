libfoo = shared_library('foo', 'libfoo.c', link_args: mlibc_link_args)
test_link_with = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: mlibc_link_args)
test_native_link_with = [libfoo_native]
