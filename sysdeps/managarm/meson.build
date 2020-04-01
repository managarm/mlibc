
protoc = find_program('protoc')
pb2frigg = find_program('pb2frigg')
proto_gen = generator(protoc, arguments: [
		'--plugin=protoc-gen-frigg=' + pb2frigg.path(),
		'--frigg_out=@BUILD_DIR@',
		'@EXTRA_ARGS@',
		'@INPUT@'
	],
	output: '@BASENAME@.frigg_pb.hpp')
fs_pb = proto_gen.process(meson.current_source_dir()
		+ '/../../subprojects/managarm/protocols/fs/fs.proto',
	extra_args: ['--proto_path=' + meson.current_source_dir()
		+ '/../../subprojects/managarm/protocols/fs'])
posix_pb = proto_gen.process(meson.current_source_dir()
		+ '/../../subprojects/managarm/protocols/posix/posix.proto',
	extra_args: ['--proto_path=' + meson.current_source_dir()
		+ '/../../subprojects/managarm/protocols/posix'])

managarm_incl = include_directories(
		'../../subprojects/managarm/hel/include',
		'../../subprojects/managarm/tools/pb2frigg/include')

rtdl_include_dirs += managarm_incl
rtdl_sources += files(
	'generic/ensure.cpp',
	'generic/memory.cpp',
	'rtdl-generic/support.cpp',
)
rtdl_sources += [
	fs_pb,
	posix_pb,
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
	fs_pb,
	posix_pb,
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
		'include/abi-bits/socket.h',
		subdir: 'abi-bits'
	)
endif

if not headers_only
	custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-src/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

