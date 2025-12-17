if not linux_eventfd_option
	subdir_done()
endif

libc_sources += files(
	'generic/sys-eventfd.cpp',
)

if not no_headers
	install_headers(
		'include/sys/eventfd.h',
		subdir: 'sys'
	)
endif

