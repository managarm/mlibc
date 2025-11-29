libfoo = shared_library('foo', 'libfoo.c', link_args: mlibc_link_args)
libbar = shared_library('bar', 'libbar.c', link_args: mlibc_link_args)
test_link_with = [libfoo, libbar]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: mlibc_link_args)
libbar_native = shared_library('native-bar', 'libbar.c', native: true, link_args: mlibc_link_args)
test_native_link_with = [libfoo_native, libbar_native]
