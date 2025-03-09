sysdep_supported_options = {
	'posix': true,
	'linux': false,
	'glibc': true,
	'bsd': true,
}

rtld_sources += files(
	'generic/generic.cpp'
)

libc_sources += files(
	'generic/entry.cpp',
	'generic/ptrace.cpp',
	'generic/generic.cpp',
	'generic/mntent.cpp',
	'generic/mount.cpp',
	'generic/reboot.cpp',
	'generic/pty.cpp',
	'generic/thread.cpp',
	'generic/mac.cpp',
	'generic/thread.S',
	'generic/utmpx.cpp'
)

if not no_headers
	install_headers(
		'include/abi-bits/access.h',
		'include/abi-bits/auxv.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/in.h',
		'include/abi-bits/reboot.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/signal.h',
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
		'include/abi-bits/utsname.h',
		'include/abi-bits/ptrace.h',
		'include/abi-bits/poll.h',
		'include/abi-bits/packet.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/ipc.h',
		'include/abi-bits/shm.h',
		'include/abi-bits/mqueue.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/statvfs.h',
		'include/abi-bits/ioctls.h',
		'include/abi-bits/msg.h',
		'include/abi-bits/rlim_t.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/sigevent.h',
		subdir: 'abi-bits',
		follow_symlinks: true
	)
	install_headers(
		'include/sys/mac.h',
		'include/sys/syscall.h',
		'include/sys/reboot.h',
		'include/sys/mount.h',
		'include/sys/sysmacros.h',
		'include/sys/ptrace.h',
		subdir: 'sys'
	)

	install_headers(
		'include/asm/ioctls.h',
		subdir: 'asm'
	)

	install_headers(
		'include/linux/fb.h',
		'include/linux/kvm.h',
		subdir: 'linux'
	)

	install_headers(
		'include/mntent.h',
		'include/pty.h',
		'include/utmpx.h'
	)
endif

if not headers_only
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-' + host_machine.cpu_family() / 'crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crti',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-' + host_machine.cpu_family() / 'crti.S',
		output: 'crti.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crtn',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-' + host_machine.cpu_family() / 'crtn.S',
		output: 'crtn.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

