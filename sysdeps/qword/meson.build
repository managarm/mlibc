
rtdl_sources += files(
	'generic/generic.cpp'
)

libc_sources += files(
	'generic/entry.cpp',
	'generic/generic.cpp',
	'generic/perfmon.cpp'
)

if not no_headers
	install_headers(
		'include/abi-bits/abi.h',
		'include/abi-bits/auxv.h',
		'include/abi-bits/vm-flags.h',
		subdir: 'abi-bits'
	)
	install_headers(
		'include/qword/perfmon.h',
		subdir: 'qword'
	)
endif

if not headers_only
	custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

