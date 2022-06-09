rtdl_test_cases = [
	'dl_iterate_phdr',
	'noload-promote',
	'rtld_next',
	'soname',
	'preinit',
	'scope1',
	'scope2',
	'scope3',
	'scope4',
]

foreach test_name : rtdl_test_cases 
	test_rpath = meson.build_root() / 'tests' / 'rtdl' / test_name / ''
	test_rpath += ':$ORIGIN/' # Workaround old and buggy qemu-user on CI

	test_link_with = []
	test_depends = []
	test_native_link_with = []
	test_native_depends = []
	test_additional_link_args = []

	# Build the needed DSOs for the test. This sets the variables above.
	subdir(test_name)

	exec = executable('rtdl-' + test_name, [test_name / 'test.c', test_sources],
		link_with: test_link_with + libc, include_directories: libc_include_dirs,
		dependencies: rtlib_deps,
		build_rpath: test_rpath,
		override_options: test_override_options,
		c_args: test_c_args,
		link_args: test_link_args + test_additional_link_args,
	)
	test(test_name, exec, suite: 'rtdl', depends: test_depends)

	if build_tests_host_libc and not host_libc_excluded_test_cases.contains(test_name)
		exec = executable('host-libc-' + test_name, test_name / 'test.c',
			link_with: test_native_link_with,
			dependencies: rtlib_deps,
			build_rpath: test_rpath,
			# Don't use ASan here, due to a bug that breaks dlopen() + DT_RUNPATH:
			# https://bugzilla.redhat.com/show_bug.cgi?id=1449604
			override_options: 'b_sanitize=undefined',
			c_args: ['-D_GNU_SOURCE', '-DUSE_HOST_LIBC'],
			link_args: ['-ldl'] + test_additional_link_args,
			native: true,
		)
		test(test_name, exec, suite: ['host-libc', 'rtdl'], depends: test_native_depends)
	endif
endforeach
