if disable_iconv_option
	subdir_done()
endif
libc_sources += files(
	'generic/iconv-stubs.cpp',
)

if not no_headers
	install_headers(
		'include/iconv.h',
	)
endif
