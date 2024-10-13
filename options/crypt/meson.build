if disable_crypt_option
	subdir_done()
endif

libc_sources += files(
	'generic/crypt.cpp',
)

if not no_headers
	install_headers(
		'include/crypt.h',
	)
endif
