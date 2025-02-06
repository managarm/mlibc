patchelf = find_program('patchelf', required: false)

if patchelf.found()
	# Overwrite LD_LIBRARY_PATH to make meson not fix our shenanigans
	# Otherwise, meson's LD_LIBRARY_PATH makes this test useless because libfoo
	# will be found through it instead of failing when libfoo is not found
	# test_env.unset('LD_LIBRARY_PATH')
	# test_native_env.unset('LD_LIBRARY_PATH')

	libfoo = shared_library('foo', 'libfoo.c')
	libbar_unpatched = shared_library('bar-unpatched', 'libbar.c',
		dependencies: libc_dep, link_with: libfoo)
	libbar = custom_target('patch-libbar',
		command: [patchelf,
			'--remove-rpath',
			'--set-soname', 'libbar.so',
			libbar_unpatched,
			'--output', '@OUTPUT0@'],
		output: ['libbar.so'],
	)
	test_link_with = [libbar, libfoo]

	libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
	libbar_native_unpatched = shared_library('native-bar-unpatched', 'libbar.c',
		dependencies: rtlib_deps, link_with: libfoo_native, native: true)
	libbar_native = custom_target('patch-libnative-bar',
		command: [patchelf,
			'--remove-rpath',
			'--set-soname', 'libnative-bar.so',
			libbar_native_unpatched,
			'--output', '@OUTPUT0@'],
		output: ['libnative-bar.so'],
	)
	test_native_link_with = [libbar_native, libfoo_native]
else
	test_skipped = true
endif
