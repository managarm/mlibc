test_additional_link_args += ['-pthread']

libfoo = shared_library('concurrent-foo', 'libfoo.c', dependencies: libc_dep,
	c_args: freestanding_hdrs_args,
	link_args: mlibc_link_args, override_options: 'b_sanitize=none')
test_depends = [libfoo]

libfoo_native = shared_library('native-concurrent-foo', 'libfoo.c', native: true)
test_native_depends = [libfoo_native]
