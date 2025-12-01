libfoo = shared_library('foo', 'libfoo.c', link_args: mlibc_link_args)
libbar = shared_library('bar', 'libbar.c', build_rpath: test_rpath, link_with: libfoo, link_args: mlibc_link_args)
test_depends = [libfoo, libbar]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: mlibc_link_args)
libbar_native = shared_library('native-bar', 'libbar.c', build_rpath: test_rpath, link_with: libfoo_native, native: true, link_args: mlibc_link_args)
test_native_depends = [libfoo_native, libbar_native]
