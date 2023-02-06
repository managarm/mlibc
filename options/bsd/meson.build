if disable_bsd_option
	subdir_done()
endif

libc_sources += files(
	'generic/arpa-nameser-stubs.cpp',
	'generic/ether.cpp',
	'generic/getopt.cpp',
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
endif
