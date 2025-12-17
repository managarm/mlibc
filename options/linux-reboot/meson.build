if not linux_reboot_option
	subdir_done()
endif

libc_sources += files(
	'generic/sys-reboot.cpp',
)

if not no_headers
	install_headers(
		'include/sys/reboot.h',
		subdir: 'sys'
	)
endif

