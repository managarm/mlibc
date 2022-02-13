ansi_test_cases = [
	'sscanf',
	'sprintf',
	'snprintf',
	'utf8',
	'strtol',
	'abs',
	'strverscmp',
	'strftime',
	'strchr',
	'strrchr',
	'wcsrtombs',
	'wmemcmp',
	'time',
	'wcsdup',
	'wcsncasecmp'
]

bsd_test_cases = [
	'ns_get_put'
]

posix_test_cases = [
	'inet_ntop',
	'inet_pton',
	'pthread_rwlock',
	'pthread_cond',
	'pthread_create',
	'pthread_cancel',
	'pthread_atfork',
	'pthread_cleanup',
	'pthread_kill',
	'pthread_key',
	'pwd',
	'getaddrinfo',
	'getdelim',
	'getnameinfo',
	'getservbyname',
	'getservbyport',
	'grp',
	'dprintf',
	'posix_spawn',
	'index',
	'rindex',
	'search',
	'open_memstream',
	'popen',
	'system', # This test should be in the ANSI tests, but it depends on sys/wait.h
	'sigsuspend',
	'sigaltstack',
	'realpath',
	'ffs',
	'getcwd'
]

posix_fail_test_cases = [
	'abort' # This test should be in the ANSI tests, but it depends on sigaction
]

glibc_test_cases = [
	'getopt',
	'ffsl-ffsll'
]

rtdl_test_cases = [
]

if not static
	rtdl_test_cases += 'dl_iterate_phdr'
endif

test_sources = [
	crt
]

if static
	test_sources += '../options/internal/gcc-extra/mlibc_crtbegin.S'
	test_sources += '../options/internal' / host_machine.cpu_family() / 'mlibc_crtend.S'
endif

if not disable_ansi_option
	foreach f : ansi_test_cases
		exec = executable('ansi-' + f, ['ansi/' + f + '.c', test_sources],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('ansi/' + f, exec)
	endforeach
endif

if not disable_bsd_option
	foreach f : bsd_test_cases
		exec = executable('ansi-' + f, ['bsd/' + f + '.c', test_sources],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('bsd/' + f, exec)
	endforeach
endif

if not disable_posix_option
	foreach f : posix_test_cases
		exec = executable('posix-' + f, ['posix/' + f + '.c', test_sources],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('posix/' + f, exec)
	endforeach

	foreach f : posix_fail_test_cases
		exec = executable('posix-' + f, ['posix/' + f + '.c', test_sources],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('posix/' + f, exec, should_fail: true)
	endforeach
endif

# We never disable the posix option so glibc is gated behind the posix option here.
if not disable_posix_option
	foreach f : glibc_test_cases
		exec = executable('glibc-' + f, ['glibc/' + f + '.c', test_sources],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('glibc/' + f, exec)
	endforeach
endif

foreach f : rtdl_test_cases
    exec = executable('rtdl-' + f, ['rtdl/' + f + '.c', test_sources],
	    link_with: libc, include_directories: libc_include_dirs,
	    build_rpath: meson.build_root(),
	    c_args: '-no-pie',
	    link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
		    '-no-pie'])
    test('rtdl/' + f, exec)
endforeach
