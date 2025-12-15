if not linux_timerfd_option
	subdir_done()
endif

libc_sources += files(
	'generic/sys-timerfd.cpp',
)

if not no_headers
	install_headers(
		'include/sys/timerfd.h',
		subdir: 'sys'
	)
endif

