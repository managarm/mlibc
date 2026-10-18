libfoo = shared_library('foo', 'libfoo.c',
	override_options: ['b_sanitize=none'],
	link_args: ['-Wl,-z,global'] + mlibc_link_args,
)
# libbar intentionally leaves foo() undefined.
libbar = shared_library('bar', 'libbar.c',
	override_options: ['b_sanitize=none', 'b_lundef=false'],
	link_args: mlibc_link_args,
)
test_depends = [libfoo, libbar]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: ['-Wl,-z,global'])
libbar_native = shared_library('native-bar', 'libbar.c',
	override_options: ['b_lundef=false'],
	native: true,
)
test_native_depends = [libfoo_native, libbar_native]
