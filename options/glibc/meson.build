if disable_glibc_option
	subdir_done()
endif
libc_sources += files(
	'generic/getopt-stubs.cpp',
	'generic/stdio_ext-stubs.cpp',
	'generic/sys-ioctl.cpp',
	'generic/err.cpp',
	'generic/error.cpp',
	'generic/resolv-stubs.cpp',
	'generic/shadow-stubs.cpp',
	'generic/printf.cpp',
	'generic/glibc-signal.cpp',
	'generic/execinfo.cpp',
	'generic/string.cpp',
	'generic/personality.cpp',
	'generic/gshadow.cpp',
	'generic/sys-timex.cpp',
)

if not no_headers
	install_headers(
		'include/getopt.h',
		'include/stdio_ext.h',
		'include/err.h',
		'include/error.h',
		'include/paths.h',
		'include/sysexits.h',
		'include/resolv.h',
		'include/endian.h',
		'include/ar.h',
		'include/shadow.h',
		'include/memory.h',
		'include/printf.h',
		'include/gshadow.h',
		'include/execinfo.h',
		'include/features.h'
	)
	install_headers(
		'include/sys/dir.h',
		'include/sys/ioctl.h',
		'include/sys/user.h',
		'include/sys/procfs.h',
		'include/sys/reg.h',
		'include/sys/errno.h',
		'include/sys/signal.h',
		'include/sys/ucontext.h',
		'include/sys/personality.h',
		'include/sys/timeb.h',
		'include/sys/mtio.h',
		'include/sys/endian.h',
		'include/sys/timex.h',
		subdir: 'sys'
	)
	install_headers(
		'include/net/ethernet.h',
		'include/net/route.h',
		'include/net/if_ppp.h',
		subdir: 'net'
	)
	install_headers(
		'include/netax25/ax25.h',
		subdir: 'netax25'
	)
	install_headers(
		'include/netipx/ipx.h',
		subdir: 'netipx'
	)
	install_headers(
		'include/netrom/netrom.h',
		subdir: 'netrom'
	)
	install_headers(
		'include/netinet/in_systm.h',
		'include/netinet/icmp6.h',
		subdir: 'netinet'
	)
	install_headers(
		'include/bits/glibc/glibc_signal.h',
		subdir: 'bits/glibc'
	)
endif

