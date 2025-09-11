if get_option('posix_option').allowed()
	libc_sources += files(
		'thread_entry.S'
    )
endif

if not headers_only
  crt = custom_target(
    'crt0',
    build_by_default: true,
    command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
    input: 'crt0.S',
    output: 'crt0.o',
    install: true,
    install_dir: get_option('libdir'),
  )

  custom_target(
    'crti',
    build_by_default: true,
    command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
    input: 'crti.S',
    output: 'crti.o',
    install: true,
    install_dir: get_option('libdir'),
  )

  custom_target(
    'crtn',
    build_by_default: true,
    command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
    input: 'crtn.S',
    output: 'crtn.o',
    install: true,
    install_dir: get_option('libdir'),
  )
endif
