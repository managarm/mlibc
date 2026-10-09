if not bsd_option
	subdir_done()
endif

libc_sources += files(
	'generic/arpa-nameser.cpp',
	'generic/ether.cpp',
	'generic/getopt.cpp',
	'generic/bsd_stdlib.cpp',
	'generic/pty.cpp',
)

if not no_headers
	# These functions conflict with Linux APIs; ABI 7 shipped the header with pthread_attr_get_np only.
	if not linux_option or mlibc_abi < 8
		install_headers('include/pthread_np.h')
	endif
	
	install_headers(
		'include/fstab.h',
		'include/pty.h',
		'include/utmp.h',
	)
	install_headers(
		'include/arpa/nameser.h',
		'include/arpa/nameser_compat.h',
		'include/arpa/telnet.h',
		subdir: 'arpa'
	)
	install_headers(
		'include/sys/queue.h',
		subdir: 'sys'
	)
	install_headers(
		'include/netinet/ether.h',
		subdir: 'netinet'
	)
	install_headers(
		'include/bits/bsd/bsd_stdlib.h',
		subdir: 'bits/bsd'
	)
endif
