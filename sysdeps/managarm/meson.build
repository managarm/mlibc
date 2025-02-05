sysdep_supported_options = {
	'posix': true,
	'linux': true,
	'glibc': true,
	'bsd': true,
}

managarm = subproject('managarm', default_options: [ 'provide_deps=true' ])
hel_dep = managarm.get_variable('hel_dep')
proto_posix_dep = managarm.get_variable('posix_extra_dep')

bragi = find_program('bragi')
bragi_dep = subproject('bragi', default_options: ['install_headers=false']).get_variable('bragi_dep')

libdrm_dep = subproject('libdrm-headers').get_variable('libdrm_dep')

bragi_gen = generator(bragi, arguments: [
		'@INPUT@',
		'-o',
		'@OUTPUT@',
		'cpp', '-l', 'frigg',
		'--protobuf',
	],
	output: '@BASENAME@.frigg_bragi.hpp')

fs_bragi = bragi_gen.process(managarm.get_variable('fs_bragi_files'))
posix_bragi = bragi_gen.process(managarm.get_variable('posix_bragi_files'))

rtld_sources += files(
	'generic/ensure.cpp',
	'generic/memory.cpp',
	'rtld-generic/support.cpp',
)
rtld_sources += [
	fs_bragi,
	posix_bragi,
]

libc_deps += [ bragi_dep, hel_dep, proto_posix_dep, libdrm_dep ]
rtld_deps += [ bragi_dep, hel_dep, proto_posix_dep ]

libc_sources += files(
	'generic/drm.cpp',
	'generic/ensure.cpp',
	'generic/entry.cpp',
	'generic/file.cpp',
	'generic/fork-exec.cpp',
	'generic/ioctl.cpp',
	'generic/memory.cpp',
	'generic/mount.cpp',
	'generic/net.cpp',
	'generic/sched.cpp',
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
elif host_machine.cpu_family() == 'riscv64'
	libc_sources += files(
		'riscv64/thread.cpp',
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
		'include/abi-bits/wait.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/utsname.h',
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
		'include/abi-bits/vt.h',
		'include/abi-bits/random.h',
		'include/abi-bits/rlim_t.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/sigevent.h',
		subdir: 'abi-bits',
		follow_symlinks: true
	)
endif

if not headers_only
	crtstuff = ['crt0']
	if host_machine.cpu_family() in ['x86_64', 'aarch64', 'riscv64']
		crtstuff += [
			'Scrt1',
			'crti',
			'crtn'
		]
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
			install_dir: get_option('libdir')
		)
	endforeach

endif

