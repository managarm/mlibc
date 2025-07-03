# meson does not have native custom_target, so when cross compiling it gets
# unhappy that we're trying to link a "host" library (which is actually built
# for the build machine) to a build executable. This means we can't patchelf
# native libraries if we want cross compilation to work.

patchelf = find_program('patchelf', required: false)

if patchelf.found()
	# Overwrite LD_LIBRARY_PATH to make meson not fix our shenanigans
	# Otherwise, meson's LD_LIBRARY_PATH makes this test useless because libfoo
	# will be found through it instead of failing when libfoo is not found
	test_env += ['LD_LIBRARY_PATH=']
	test_native_env += ['LD_LIBRARY_PATH=']

	libfoo = shared_library('foo', 'libfoo.c', link_args: test_additional_link_args)
	libbar_unpatched = shared_library('bar-unpatched', 'libbar.c',
		link_args: test_additional_link_args,
		override_options: 'b_sanitize=none',
		dependencies: libc_dep,
		link_with: libfoo)
	libbar = custom_target('patch-libbar',
		command: [patchelf,
			'--remove-rpath',
			'--set-soname', 'libbar.so',
			libbar_unpatched,
			'--output', '@OUTPUT0@'],
		output: ['libbar.so'],
	)
	test_link_with = [libbar, libfoo]
else
	test_skipped = true
endif
