if not static
	rtdl_sources += files(
		'generic/sysdeps.cpp',
	)
endif

libc_sources += files(
	'x86_64/signals.S',
	'generic/entry.cpp',
	'generic/sysdeps.cpp',
)

if not disable_posix_option
	libc_sources += files(
		'x86_64/cp_syscall.S',
		'x86_64/thread.cpp',
		'x86_64/thread_entry.S'
	)
endif

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
		input: 'x86_64/crt-src/crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('Scrt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64/crt-src/Scrt1.S',
		output: 'Scrt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crti',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64/crt-src/crti.S',
		output: 'crti.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crtn',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64/crt-src/crtn.S',
		output: 'crtn.o',
		install: true,
		install_dir: get_option('libdir')
	)

	wrapper_conf = configuration_data()
	wrapper_conf.set('LIBDIR', get_option('libdir'))
	wrapper_conf.set('PREFIX', get_option('prefix'))
	specs = configure_file(input: 'mlibc-gcc.specs.in',
			output: 'mlibc-gcc.specs',
			configuration: wrapper_conf)

	wrapper_script = configure_file(input: 'mlibc-gcc.in',
			output: 'mlibc-gcc',
			configuration: wrapper_conf)

	install_data(specs,
		install_dir: get_option('libdir')
	)

	install_data(wrapper_script,
		install_dir: get_option('bindir')
	)
endif
