if not linux_signalfd_option
	subdir_done()
endif

libc_sources += files(
	'generic/sys-signalfd.cpp',
)

if not no_headers
	install_headers(
		'include/sys/signalfd.h',
		subdir: 'sys'
	)
endif

