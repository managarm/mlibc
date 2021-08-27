sigma_incl = include_directories('../../subprojects/libsigma/include')
protocols_incl = include_directories('../../../../../build/system-root/usr/include')


rtdl_include_dirs += [sigma_incl, protocols_incl]
rtdl_sources += files(
	'generic/log.cpp',
	'generic/memory.cpp',
	'rtdl-generic/support.cpp',
)

libc_include_dirs += [sigma_incl, protocols_incl]

libc_sources += files(
	'generic/log.cpp',
	'generic/entry.cpp',
	'generic/memory.cpp',
	'generic/generic.cpp',
	'generic/file.cpp'
)

if not no_headers
	install_headers(
		'include/abi-bits/abi.h',
		'include/abi-bits/auxv.h',
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
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-src/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

