ansi_sources = files(
	'generic/stdlib.cpp',
	'generic/assert.cpp',
	'generic/ctype.cpp',
	'generic/environment.cpp',
	'generic/errno.cpp',
	'generic/file-io.cpp',
	'generic/inttypes.cpp',
	'generic/locale.cpp',
	'generic/signal.cpp',
	'generic/stdio.cpp',
	'generic/stdlib.cpp',
	'generic/string.cpp',
	'generic/threads.cpp',
	'generic/time.cpp',
	'generic/uchar.cpp',
	'generic/wchar.cpp',
	'generic/wctype.cpp',
)

if not no_headers
	install_headers(
		'include/alloca.h',
		'include/assert.h',
		'include/ctype.h',
		'include/errno.h',
		'include/inttypes.h',
		'include/limits.h',
		'include/locale.h',
		'include/setjmp.h',
		'include/signal.h',
		'include/stdc-predef.h',
		'include/stdio.h',
		'include/stdlib.h',
		'include/string.h',
		'include/threads.h',
		'include/time.h',
		'include/uchar.h',
		'include/wchar.h',
		'include/wctype.h',
	)
	install_headers(
		'include/bits/ansi/timespec.h',
		'include/bits/ansi/time_t.h',
		'include/bits/ansi/clock_t.h',
		subdir: 'bits/ansi'
	)
endif
