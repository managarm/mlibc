if not linux_epoll_option
	subdir_done()
endif

libc_sources += files(
	'generic/epoll.cpp',
)

if not no_headers
	install_headers(
		'include/sys/epoll.h',
		subdir: 'sys'
	)
endif

