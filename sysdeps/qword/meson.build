
disable_linux_option = true
internal_conf.set('MLIBC_MAP_DSO_SEGMENTS', false)

rtdl_include_dirs += include_directories('include')
rtdl_sources += files(
	'generic/file.cpp'
)

libc_include_dirs += include_directories('include')
libc_sources += files(
	'generic/entry.cpp',
	'generic/file.cpp'
)

install_headers(
	'include/abi-bits/abi.h',
	'include/abi-bits/auxv.h',
	subdir: 'abi-bits'
)

custom_target('crt0',
	build_by_default: true,
	command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
	input: 'crt-x86_64/crt0.S',
	output: 'crt0.o',
	install: true,
	install_dir: get_option('libdir')
)

