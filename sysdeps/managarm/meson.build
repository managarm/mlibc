
protoc = find_program('protoc')
frigg_pb = find_program('frigg_pb')
proto_gen = generator(protoc, arguments: [
		'--plugin=protoc-gen-frigg=' + frigg_pb.path(),
		'--frigg_out=@BUILD_DIR@',
		'--proto_path=@CURRENT_SOURCE_DIR@/../../subprojects/managarm/bragi/proto',
		'@INPUT@'
	],
	output: '@BASENAME@.frigg_pb.hpp')
fs_pb = proto_gen.process('../../subprojects/managarm/bragi/proto/fs.proto')
posix_pb = proto_gen.process('../../subprojects/managarm/bragi/proto/posix.proto')

managarm_incl = include_directories('../../subprojects/managarm/frigg/include',
		'../../subprojects/managarm/hel/include')

rtdl_include_dirs += include_directories('include')
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

libc_include_dirs += include_directories('include',
	'../../../ports/libdrm/include')
libc_include_dirs += managarm_incl

libc_sources += files(
	'generic/ensure.cpp',
	'generic/entry.cpp',
	'generic/file.cpp',
	'generic/fork-exec.cpp',
	'generic/frigg-support.cpp',
	'generic/memory.cpp',
	'generic/mount.cpp',
	'generic/signals.cpp',
	'generic/socket.cpp',
	'generic/time.cpp',
	'x86_64/signals.S',
)
libc_sources += [
	fs_pb,
	posix_pb,
]

install_headers(
	'include/abi-bits/abi.h',
	'include/abi-bits/auxv.h',
	subdir: 'bits'
)

