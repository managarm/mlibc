libc_sources += files(
	'generic/startup.cpp',
)

if sysdep_abi == 'sysv'
	libc_sources += files(
		'generic/phdr.cpp',
	)
endif

if not no_headers
	install_headers(
		'include/elf.h',
		'include/link.h',
	)
endif
