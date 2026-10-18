libfoo = shared_library('foo', 'libfoo.c',
	override_options: ['b_sanitize=none'],
	link_args: mlibc_link_args,
)
libinterpose = shared_library('interpose', 'libinterpose.c',
	override_options: ['b_sanitize=none'],
	link_args: ['-Wl,-z,interpose'] + mlibc_link_args,
)
test_link_with = [libfoo, libinterpose]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
libinterpose_native = shared_library('native-interpose', 'libinterpose.c',
	link_args: ['-Wl,-z,interpose'],
	native: true,
)
test_native_link_with = [libfoo_native, libinterpose_native]
