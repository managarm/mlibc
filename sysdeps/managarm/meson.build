
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
	'generic/memory.cpp',
	'generic/mount.cpp',
	'generic/signals.cpp',
	'generic/socket.cpp',
	'generic/time.cpp',
	'x86_64/signals.S',
	'x86_64/thread_entry.S',
	'x86_64/thread.cpp'
)
libc_sources += [
	fs_bragi,
	posix_bragi,
]

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

