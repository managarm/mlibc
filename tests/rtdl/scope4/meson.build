libbaz = shared_library('baz', 'libbaz.c')
libfoo = shared_library('foo', 'libfoo.c', build_rpath: test_rpath, link_with: libbaz)
libbar = shared_library('bar', 'libbar.c', build_rpath: test_rpath, link_with: libfoo)
test_depends = [libfoo, libbar, libbaz]

libbaz_native = shared_library('native-baz', 'libbaz.c', native: true)
libfoo_native = shared_library('native-foo', 'libfoo.c', build_rpath: test_rpath, link_with: libbaz_native, native: true)
libbar_native = shared_library('native-bar', 'libbar.c', build_rpath: test_rpath, link_with: libfoo_native, native: true)
test_native_depends = [libfoo_native, libbar_native, libbaz_native]
