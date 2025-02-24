if not bsd_option
	subdir_done()
endif

libc_sources += files(
	'generic/arpa-nameser.cpp',
	'generic/ether.cpp',
	'generic/getopt.cpp',
	'generic/bsd_stdlib.cpp',
)

if not no_headers
	install_headers(
		'include/fstab.h',
	)
	install_headers(
		'include/arpa/nameser.h',
		'include/arpa/nameser_compat.h',
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
		'include/bits/bsd/bsd_unistd.h',
		subdir: 'bits/bsd'
	)
endif
