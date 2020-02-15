
rtdl_sources += files(
)

libc_sources += files(
	'generic/filesystem.cpp',
	'generic/syscall.c',
	'generic/lemon.cpp',
)

if not no_headers
	install_headers(
		'include/abi-bits/abi.h',
		'include/abi-bits/auxv.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		subdir: 'abi-bits'
	)
	install_headers(
		'include/lemon/syscall.h',
		'include/lemon/spawn.h',
		subdir: 'lemon'
	)
endif

if not headers_only
	custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

