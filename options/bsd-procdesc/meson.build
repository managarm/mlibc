if not bsd_procdesc_option
	subdir_done()
endif

libc_sources += files(
	'generic/sys-procdesc.cpp',
)

if not no_headers
	install_headers(
		'include/sys/procdesc.h',
		subdir: 'sys'
	)
endif

