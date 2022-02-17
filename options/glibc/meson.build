
libc_sources += files(
	'generic/getopt-stubs.cpp',
	'generic/stdio_ext-stubs.cpp',
	'generic/sys-ioctl.cpp',
	'generic/err.cpp',
	'generic/resolv-stubs.cpp',
	'generic/shadow-stubs.cpp',
	'generic/printf.cpp',
)

if not no_headers
	install_headers(
		'include/getopt.h',
		'include/stdio_ext.h',
		'include/err.h',
		'include/paths.h',
		'include/sysexits.h',
		'include/resolv.h',
		'include/endian.h',
		'include/ar.h',
		'include/shadow.h',
		'include/memory.h',
		'include/printf.h',
		'include/gshadow.h'
	)
	install_headers(
		'include/sys/dir.h',
		'include/sys/ioctl.h',
		subdir: 'sys'
	)
	install_headers(
		'include/net/ethernet.h',
		subdir: 'net'
	)
	install_headers(
		'include/netinet/in_systm.h',
		subdir: 'netinet'
	)
endif

