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
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/in.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/reboot.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/time.h',
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

	custom_target('crti',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64-crt/crti.S',
		output: 'crti.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crtn',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64-crt/crtn.S',
		output: 'crtn.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
