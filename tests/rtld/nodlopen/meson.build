libfoo = shared_library('foo', 'libfoo.c',
	override_options: ['b_sanitize=none'],
	link_args: ['-Wl,-z,nodlopen'] + mlibc_link_args,
)
test_depends = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: ['-Wl,-z,nodlopen'])
test_native_depends = [libfoo_native]
