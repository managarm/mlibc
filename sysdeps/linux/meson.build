sysdep_supported_options = {
	'posix': true,
	'linux': true,
	'glibc': true,
	'bsd': true,
}

rtld_dso_sources += files(
	host_machine.cpu_family() / 'arch-syscall.cpp',
	'generic/sysdeps.cpp',
)

linux_include_dirs = [
	include_directories(host_machine.cpu_family()),
	include_directories('include-internal/'),
]

rtld_include_dirs += linux_include_dirs
libc_include_dirs += linux_include_dirs
libc_sources += files(
	host_machine.cpu_family() / 'signals.S',
	host_machine.cpu_family() / 'arch-syscall.cpp',
	'generic/entry.cpp',
	'generic/sysdeps.cpp',
)

if get_option('posix_option').allowed()
	libc_sources += files(
		'generic/thread.cpp',
		host_machine.cpu_family() / 'cp_syscall.S',
		host_machine.cpu_family() / 'thread_entry.S'
	)
endif

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
		'include/abi-bits/ipc.h',
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
		'include/abi-bits/random.h',
		'include/abi-bits/rlim_t.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/sigevent.h',
		subdir: 'abi-bits',
		follow_symlinks: true
	)

	install_headers('include/syscall.h')
	install_headers('include/sys/syscall.h', subdir: 'sys')
	install_headers(
		'include/bits/syscall.h',
		'include/bits/syscall_aliases.h',
		subdir: 'bits'
	)
endif

if not headers_only
	crt = custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: host_machine.cpu_family() / 'crt-src/crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	crt_pie = custom_target('Scrt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-fPIE', '-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: host_machine.cpu_family() / 'crt-src/Scrt1.S',
		output: 'Scrt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	# On RISC-V and LoongArch, crti.o and crtn.o are provided by GCC.
	if host_machine.cpu_family() != 'riscv64' and host_machine.cpu_family() != 'loongarch64'
		custom_target('crti',
			build_by_default: true,
			command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: host_machine.cpu_family() / 'crt-src/crti.S',
			output: 'crti.o',
			install: true,
			install_dir: get_option('libdir')
		)

		custom_target('crtn',
			build_by_default: true,
			command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: host_machine.cpu_family() / 'crt-src/crtn.S',
			output: 'crtn.o',
			install: true,
			install_dir: get_option('libdir')
		)
	endif

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
