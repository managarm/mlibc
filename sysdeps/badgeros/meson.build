sysdep_supported_options = {
	'posix': true,
}

rtld_sources += files(
	'sysdeps.cpp',
	'syscall.cpp',
)

libc_sources += files(
	'entry.cpp',
	'sysdeps.cpp',
	'syscall.cpp',
)

if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/riscv-hwprobe.h',
		'include/abi-bits/sigevent.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/sa_family_t.h',
		'include/abi-bits/sockaddr_storage.h',
		'include/abi-bits/sig-limits.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/useconds_t.h',
		'include/abi-bits/access.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/poll.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/in.h',
		'include/abi-bits/rlim_t.h',
		'include/abi-bits/utsname.h',
		'include/abi-bits/fd_set.h',
		'include/abi-bits/sem.h',
		'include/abi-bits/time.h',
		'include/abi-bits/ipc.h',
		'include/abi-bits/statvfs.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/shm.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/msg.h',
		'include/abi-bits/mqueue.h',
		'include/abi-bits/utmp-defines.h',
		'include/abi-bits/utmpx.h',
		'include/abi-bits/sigset_t.h',
		subdir: 'abi-bits',
		follow_symlinks: true
	)
endif

if not headers_only
	crt = custom_target('Scrt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'Scrt0.S',
		output: 'Scrt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
	crt = custom_target('crti',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crti.S',
		output: 'crti.o',
		install: true,
		install_dir: get_option('libdir')
	)
	crt = custom_target('crtn',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crtn.S',
		output: 'crtn.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
