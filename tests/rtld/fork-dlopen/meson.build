test_additional_link_args += ['-pthread']

libanchor = shared_library('fork-anchor', 'libanchor.c', dependencies: libc_dep,
	c_args: freestanding_hdrs_args,
	link_args: mlibc_link_args, override_options: 'b_sanitize=none')
libchild = shared_library('fork-child', 'libchild.c', dependencies: libc_dep,
	c_args: freestanding_hdrs_args,
	link_args: mlibc_link_args, override_options: 'b_sanitize=none')
test_depends = [libanchor, libchild]

libanchor_native = shared_library('native-fork-anchor', 'libanchor.c', native: true)
libchild_native = shared_library('native-fork-child', 'libchild.c', native: true)
test_native_depends = [libanchor_native, libchild_native]
