rtdl_sources += files(
	'generic/sysdeps.cpp',
)

libc_sources += files(
	'generic/entry.cpp',
	'generic/sysdeps.cpp',
)

if not no_headers
	install_headers(
		'include/abi-bits/abi.h',
		'include/abi-bits/auxv.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		subdir: 'abi-bits'
	)
endif

if not headers_only
	custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64-crt/crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('Scrt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64-crt/Scrt1.S',
		output: 'Scrt1.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
