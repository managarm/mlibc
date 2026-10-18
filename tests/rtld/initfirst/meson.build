libbar = shared_library('bar', 'libbar.c',
	dependencies: libc_dep,
	override_options: ['b_sanitize=none'],
	link_args: mlibc_link_args,
)
libfoo = shared_library('foo', 'libfoo.c',
	dependencies: libc_dep,
	link_with: libbar,
	override_options: ['b_sanitize=none'],
	link_args: ['-Wl,-z,initfirst'] + mlibc_link_args,
)
test_link_with = [libfoo, libbar]

libbar_native = shared_library('native-bar', 'libbar.c', native: true)
libfoo_native = shared_library('native-foo', 'libfoo.c',
	link_with: libbar_native,
	link_args: ['-Wl,-z,initfirst'],
	native: true,
)
test_native_link_with = [libfoo_native, libbar_native]
