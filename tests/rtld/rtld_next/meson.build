# Prevent tail calls, as it breaks libc's 'calling object' detection.
no_tail_calls = '-fno-optimize-sibling-calls'

libfoo = shared_library('foo', 'libfoo.c',
	dependencies: libc_dep,
	c_args: no_tail_calls,
)
libbar = shared_library('bar', 'libbar.c',
	dependencies: libc_dep,
	c_args: no_tail_calls,
)
test_link_with = [libfoo, libbar] # foo is linked before bar

libfoo_native = shared_library('native-foo', 'libfoo.c',
	c_args: [no_tail_calls, '-D_GNU_SOURCE'],
	link_args: '-ldl',
	override_options: 'b_sanitize=none',
	native: true
)
libbar_native = shared_library('native-bar', 'libbar.c',
	c_args: [no_tail_calls, '-D_GNU_SOURCE'],
	link_args: '-ldl',
	override_options: 'b_sanitize=none',
	native: true
)
test_native_link_with = [libfoo_native, libbar_native]

