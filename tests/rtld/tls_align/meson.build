libfoo = shared_library('foo', 'libfoo.c')
libbar = shared_library('bar', 'libbar.c')
test_link_with = [libfoo, libbar]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
libbar_native = shared_library('native-bar', 'libbar.c', native: true)
test_native_link_with = [libfoo_native, libbar_native]
