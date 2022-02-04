if disable_bsd_option
	subdir_done()
endif

libc_sources += files(
	'generic/arpa-nameser-stubs.cpp',
)

if not no_headers
	install_headers(
		'include/arpa/nameser.h',
		subdir: 'arpa'
	)
endif
