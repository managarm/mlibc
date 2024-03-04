# Create two libraries with the same SONAME.
bar_soname = '-Wl,-soname=libbar.so'

libfoo = shared_library('foo', 'libfoo.c', link_args: '-Wl,-soname=libbar.so')
libbar = shared_library('bar', 'libbar.c', link_args: '-Wl,-soname=libbar.so')
test_depends = [libfoo, libbar]

libfoo_native = shared_library('native-foo', 'libfoo.c',
	link_args: ['-ldl', '-Wl,-soname=libnative-bar.so'],
	native: true
)
libbar_native = shared_library('native-bar', 'libbar.c',
	link_args: ['-ldl', '-Wl,-soname=libnative-bar.so'],
	native: true
)
test_native_depends = [libfoo_native, libbar_native]

