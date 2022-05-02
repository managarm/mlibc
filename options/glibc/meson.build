if disable_glibc_option
	subdir_done()
endif
libc_sources += files(
	'generic/getopt-stubs.cpp',
	'generic/stdio_ext-stubs.cpp',
	'generic/sys-ioctl.cpp',
	'generic/err.cpp',
	'generic/resolv-stubs.cpp',
	'generic/shadow-stubs.cpp',
	'generic/printf.cpp',
	'generic/glibc-signal.cpp',
	'generic/execinfo.cpp',
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
		'include/gshadow.h',
		'include/execinfo.h'
	)
	install_headers(
		'include/sys/dir.h',
		'include/sys/ioctl.h',
		'include/sys/user.h',
		'include/sys/procfs.h',
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
	install_headers(
		'include/bits/glibc/glibc_signal.h',
		subdir: 'bits/glibc'
	)
endif

