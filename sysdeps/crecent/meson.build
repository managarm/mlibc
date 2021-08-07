rtdl_sources = files(
	'generic/generic.cpp'
)

libc_sources = files(
	'generic/entry.cpp',
	'generic/generic.cpp'
)

if not no_headers
	install_headers(
		'include/crcnt/syscall.h',
		subdir: 'crcnt'
	)
endif

if not headers_only
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array()  ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
