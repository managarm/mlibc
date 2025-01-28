if host_machine.cpu_family() == 'riscv64'
	# gp isn't initialized until after crt1.o runs, so to access
	# globals in our pre-initializers we must disable it.
	test_additional_link_args = ['-Wl,--no-relax']
endif

libfoo = shared_library('foo', ['libfoo.c'],
	dependencies: libc_dep,
	override_options: 'b_sanitize=none',
	link_args: test_additional_link_args,
)
test_link_with = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c',
	link_args: ['-ldl'] + test_additional_link_args,
	override_options: 'b_sanitize=none',
	native: true
)
test_native_link_with = [libfoo_native]
