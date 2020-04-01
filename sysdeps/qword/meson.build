
rtdl_sources += files(
	'generic/generic.cpp'
)

libc_sources += files(
	'generic/entry.cpp',
	'generic/generic.cpp',
	'generic/memstats.cpp',
	'generic/mount.cpp'
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
		'include/abi-bits/socket.h',
		subdir: 'abi-bits'
	)
	install_headers(
		'include/qword/fcntl.h',
		'include/qword/memstats.h',
		'include/qword/mount.h',
		subdir: 'qword'
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

