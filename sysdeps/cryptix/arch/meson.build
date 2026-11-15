libc_sources += files(
	host_machine.cpu_family() / 'thread_entry.S',
)

if not headers_only
	crt = custom_target(
		'crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + freestanding_hdrs_args + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: host_machine.cpu_family() / 'crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir'),
	)

	foreach crtthing : ['crti', 'crtn']
		custom_target(
			crtthing,
			build_by_default: true,
			command: c_compiler.cmd_array() + freestanding_hdrs_args + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: host_machine.cpu_family() / crtthing + '.S',
			output: crtthing + '.o',
			install: true,
			install_dir: get_option('libdir'),
		)
	endforeach
endif
