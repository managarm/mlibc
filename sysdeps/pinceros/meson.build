rtld_dso_sources += files(
	'generic/ensure.cpp',
	'generic/file.cpp',
	'generic/fork-exec.cpp',
	'generic/memory.cpp',
)

libc_sources += files(
	'generic/ensure.cpp',
	'generic/entry.cpp',
	'generic/file.cpp',
	'generic/fork-exec.cpp',
	'generic/memory.cpp',
	'generic/time.cpp',
	'generic/pinceros.cpp',
)

if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/in.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/statx.h',
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
		'include/abi-bits/access.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/utsname.h',
		'include/abi-bits/ptrace.h',
		'include/abi-bits/vt.h',
		'include/abi-bits/ptrace.h',
		'include/abi-bits/poll.h',
		'include/abi-bits/epoll.h',
		'include/abi-bits/packet.h',
		'include/abi-bits/inotify.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/shm.h',
		'include/abi-bits/mqueue.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/statfs.h',
		'include/abi-bits/statvfs.h',
		'include/abi-bits/ioctls.h',
		'include/abi-bits/xattr.h',
		'include/abi-bits/msg.h',
		subdir: 'abi-bits',
		follow_symlinks: true,
	)

	install_headers(
		'include/' + host_machine.cpu_family() + '/syscall.h',
		'include/sys/pinceros.h',
		subdir: 'sys',
	)
endif

if library_type in ['static', 'both']
	libc_sources += files('aarch64/dso_handle.S')
endif

if not headers_only
	crtstuff = ['crt0']
	if host_machine.cpu_family() in ['x86_64', 'aarch64']
		crtstuff += ['Scrt1', 'crti', 'crtn']
	endif
	foreach crtthing : crtstuff
		crtf = crtthing + '.S'
		crt_src = files(host_machine.cpu_family() / 'crt-src' / crtf)
		crt = custom_target(
			crtthing,
			build_by_default: true,
			command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: crt_src,
			output: crtthing + '.o',
			install: true,
			install_dir: get_option('libdir'),
		)
	endforeach

endif