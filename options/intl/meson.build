if disable_intl_option
	subdir_done()
endif
libc_sources += files(
	'generic/libintl-stubs.cpp',
)

if not no_headers
	install_headers(
		'include/libintl.h',
	)
endif
