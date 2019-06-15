
libc_sources += files(
	'generic/getopt-stubs.cpp',
	'generic/stdio_ext-stubs.cpp',
	'generic/sys-ioctl.cpp',
	'generic/err.cpp',
)

if not no_headers
	install_headers(
		'include/getopt.h',
		'include/stdio_ext.h',
		'include/err.h'
	)
	install_headers(
		'include/sys/ioctl.h',
		subdir: 'sys'
	)
endif

