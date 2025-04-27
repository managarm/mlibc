rtld_test_cases = [
	'dl_iterate_phdr',
	'dladdr_local',
	'ld_library_path',
	'noload-promote',
	'rtld_next',
	'soname',
	'preinit',
	'destroy1',
	'destroy2',
	'scope1',
	'scope2',
	'scope3',
	'scope4',
	'scope5',
	'tls_align',
	'relr',
	'symver',
]

host_libc_rtld_nosan_test_cases = [
	'preinit',
]

foreach test_name : rtld_test_cases
	test_rpath = meson.global_build_root() / 'tests' / 'rtld' / test_name / ''
	test_rpath += ':$ORIGIN/' # Workaround old and buggy qemu-user on CI

	test_env = []
	test_link_with = []
	test_depends = []
	test_native_env = []
	test_native_link_with = []
	test_native_depends = []
	test_additional_link_args = []

	# Build the needed DSOs for the test. This sets the variables above.
	subdir(test_name)

	exec = executable('rtld-' + test_name, [test_name / 'test.c', test_sources],
		link_with: test_link_with,
		dependencies: libc_dep,
		build_rpath: test_rpath,
		override_options: test_override_options,
		c_args: test_c_args,
		link_args: test_link_args + test_additional_link_args,
	)
	test(test_name, exec, env: test_env, suite: 'rtld', depends: test_depends)

	if build_tests_host_libc and not host_libc_excluded_test_cases.contains(test_name)
		if test_name in host_libc_rtld_nosan_test_cases
			host_libc_rtld_sanitize_options = 'b_sanitize=none'
		else
			# Don't use ASan here, due to a bug that breaks dlopen() + DT_RUNPATH:
			# https://bugzilla.redhat.com/show_bug.cgi?id=1449604
			host_libc_rtld_sanitize_options = 'b_sanitize=undefined'
		endif

		exec = executable('host-libc-' + test_name, test_name / 'test.c',
			link_with: test_native_link_with,
			dependencies: rtlib_deps,
			build_rpath: test_rpath,
			override_options: host_libc_rtld_sanitize_options,
			c_args: ['-D_GNU_SOURCE', '-DUSE_HOST_LIBC'],
			link_args: ['-ldl'] + test_additional_link_args,
			native: true,
		)
		test(test_name, exec, env: test_native_env, suite: ['host-libc', 'rtld'], depends: test_native_depends)
	endif
endforeach
