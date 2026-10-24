libinner = shared_library('recursive-inner', 'libinner.c', dependencies: libc_dep,
	c_args: freestanding_hdrs_args,
	link_args: mlibc_link_args, override_options: 'b_sanitize=none')
libouter = shared_library('recursive-outer', 'libouter.c', dependencies: libc_dep,
	c_args: freestanding_hdrs_args,
	link_args: mlibc_link_args, build_rpath: '$ORIGIN', override_options: 'b_sanitize=none')
test_depends = [libinner, libouter]

libinner_native = shared_library('native-recursive-inner', 'libinner.c', native: true)
libouter_native = shared_library('native-recursive-outer', 'libouter.c', native: true,
	c_args: ['-DUSE_HOST_LIBC'], build_rpath: '$ORIGIN')
test_native_depends = [libinner_native, libouter_native]
