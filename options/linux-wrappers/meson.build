if not linux_wrappers_option
	subdir_done()
endif

libc_sources += files(
	'generic/signals.cpp',
)

if not no_headers
	install_headers(
		'include/mlibc/wrappers.h',
		subdir: 'mlibc'
	)
endif
