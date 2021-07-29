
rtdl_sources += files(
	'generic/generic.cpp'
)

libc_sources += files(
	'generic/entry.cpp',
	'generic/generic.cpp',
	'generic/thread.cpp',
	'generic/thread_entry.S'
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
		'include/abi-bits/resource.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/limits.h',
		subdir: 'abi-bits'
	)
endif

if not headers_only
	crt = custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

