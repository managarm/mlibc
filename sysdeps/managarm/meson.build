
bragi = find_program('bragi')

bragi_gen = generator(bragi, arguments: [
		'-l', 'frigg',
		'--protobuf',
		'@INPUT@',
		'@OUTPUT@'
	],
	output: '@BASENAME@.frigg_bragi.hpp')

fs_bragi = bragi_gen.process('../../subprojects/managarm/protocols/fs/fs.bragi')
posix_bragi = bragi_gen.process('../../subprojects/managarm/protocols/posix/posix.bragi')

managarm_incl = include_directories(
		'../../subprojects/managarm/protocols/posix/include',
		'../../subprojects/managarm/hel/include',
		'../../subprojects/bragi/include')

rtdl_include_dirs += managarm_incl
rtdl_sources += files(
	'generic/ensure.cpp',
	'generic/memory.cpp',
	'rtdl-generic/support.cpp',
)
rtdl_sources += [
	fs_bragi,
	posix_bragi,
]

libc_include_dirs += include_directories('../../../ports/libdrm/include')
libc_include_dirs += managarm_incl

libc_sources += files(
	'generic/ensure.cpp',
	'generic/entry.cpp',
	'generic/file.cpp',
	'generic/fork-exec.cpp',
	'generic/ioctl.cpp',
	'generic/memory.cpp',
	'generic/mount.cpp',
	'generic/signals.cpp',
	'generic/socket.cpp',
	'generic/time.cpp'
)
libc_sources += [
	fs_bragi,
	posix_bragi,
]

if host_machine.cpu_family() == 'aarch64'
	libc_sources += files(
		'aarch64/signals.S',
		'aarch64/thread_entry.S',
		'aarch64/thread.cpp'
	)
elif host_machine.cpu_family() == 'x86_64'
	libc_sources += files(
		'x86_64/signals.S',
		'x86_64/thread_entry.S',
		'x86_64/thread.cpp'
	)
else
	error('Unknown architecture')
endif

if not no_headers
	install_headers(
		'include/abi-bits/access.h',
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
		'include/abi-bits/utsname.h',
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
		subdir: 'abi-bits'
	)
endif

if not headers_only
	crt_src = files(host_machine.cpu_family() / 'crt-src/crt0.S')
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: crt_src,
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)

	if host_machine.cpu_family() == 'x86_64'
		crt_pie = custom_target('Scrt1',
			build_by_default: true,
			command: c_compiler.cmd_array() + ['-fPIE', '-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: 'x86_64/crt-src/Scrt1.S',
			output: 'Scrt1.o',
			install: true,
			install_dir: get_option('libdir')
		)
	endif
endif

