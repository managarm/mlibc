
rtdl_sources += files(
)

libc_sources += files(
	'generic/filesystem.c',
	'generic/filesystem.cpp',
	'generic/ipc.c',
	'generic/syscall.c',
	'generic/fb.c',
	'generic/process.c',
	'generic/lemon.c',
	'generic/lemon.cpp',
	'generic/exit.c',
	'generic/itoa.c',
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
		'include/lemon/ipc.h',
		'include/lemon/filesystem.h',
		'include/lemon/syscall.h',
		'include/lemon/types.h',
		'include/lemon/fb.h',
		'include/lemon/lemon.h',
		'include/lemon/keyboard.h',
		'include/lemon/itoa.h',
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

