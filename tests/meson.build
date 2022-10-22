all_test_cases = [
	'ansi/alloc',
	'ansi/sscanf',
	'ansi/sprintf',
	'ansi/snprintf',
	'ansi/utf8',
	'ansi/strtol',
	'ansi/abs',
	'ansi/longjmp',
	'ansi/strverscmp',
	'ansi/strftime',
	'ansi/strchr',
	'ansi/strrchr',
	'ansi/wcsrtombs',
	'ansi/wmemcmp',
	'ansi/timegm',
	'ansi/ungetc',
	'ansi/wcsdup',
	'ansi/wcsncasecmp',
	'ansi/fopen',
	'ansi/memmem',
	'bsd/ns_get_put',
	'bsd/reallocarray',
	'bsd/strl',
	'posix/inet_ntop',
	'posix/inet_pton',
	'posix/access',
	'posix/pthread_barrier',
	'posix/pthread_rwlock',
	'posix/pthread_cond',
	'posix/pthread_create',
	'posix/pthread_cancel',
	'posix/pthread_atfork',
	'posix/pthread_cleanup',
	'posix/pthread_kill',
	'posix/pthread_mutex',
	'posix/pthread_key',
	'posix/pthread_thread_local',
	'posix/pthread_attr',
	'posix/pwd',
	'posix/fdopen',
	'posix/getaddrinfo',
	'posix/getdelim',
	'posix/getnameinfo',
	'posix/getservbyname',
	'posix/getservbyport',
	'posix/grp',
	'posix/dprintf',
	'posix/posix_memalign',
	'posix/posix_spawn',
	'posix/index',
	'posix/rindex',
	'posix/search',
	'posix/open_memstream',
	'posix/popen',
	'posix/system', # This test should be in the ANSI tests, but it depends on sys/wait.h
	'posix/sigsuspend',
	'posix/sigaltstack',
	'posix/time',
	'posix/realpath',
	'posix/ffs',
	'posix/getcwd',
	'posix/memrchr',
	'posix/wordexp',
	'posix/rlimits',
	'posix/accept4',
	'posix/setpriority',
	'posix/alarm',
	'posix/abort', # This test should be in the ANSI tests, but it depends on sigaction
	'posix/timer',
	'posix/vfork',
	'posix/wcwidth',
	'posix/pause',
	'posix/flockfile',
	'posix/basename',
	'posix/regex',
	'glibc/getopt',
	'glibc/ffsl-ffsll',
	'linux/xattr',
]

if host_machine.system() == 'linux'
	all_test_cases += 'glibc/linux-syscall'
endif

fail_test_cases = [
	'posix/abort',
]

host_libc_excluded_test_cases = [
	'bsd/strl', # These functions do not exist on Linux.
]
host_libc_noasan_test_cases = [
	'posix/pthread_cancel',
	'posix/pthread_attr', # does some stack overflowing to check stack size
	'posix/posix_memalign',
	'posix/search', # requires tdelete (#351)
]

test_sources = []
test_link_args = []
test_c_args = []
use_pie = false

test_c_args = []
test_link_args = []

# Our ubsan implementation can't be used by the tests themselves,
# since it is internal to libc.so and ld.so.
test_override_options = ['b_sanitize=none']

if library_type == 'static'
	libc = libc_static
	use_pie = false
	test_c_args += '-no-pie'
	test_link_args += ['-no-pie', '-static']
	test_sources += [
		'../options/internal/gcc-extra/mlibc_crtbegin.S',
		'../options/internal' / host_machine.cpu_family() / 'mlibc_crtend.S',
		crt,
	]
else
	libc = libc_shared
	test_link_args += ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so']

	if host_machine.system() in ['linux', 'managarm']
		use_pie = true
		test_sources += crt_pie
	else
		use_pie = false
		test_sources += crt

		# Meson doesn't set these for us (issue #4651).
		test_c_args += '-no-pie'
		test_link_args += '-no-pie'
	endif

	# Add the rtdl tests.
	if not disable_posix_option
		subdir('rtdl')
	endif
endif

foreach test_name : all_test_cases
	test_subdir = test_name.split('/')[0]
	test_short_name = test_name.split('/')[1]
	test_exec_name = test_name.replace('/', '-')

	if test_subdir == 'ansi' and disable_ansi_option
		continue
	elif test_subdir == 'bsd' and disable_bsd_option
		continue
	elif test_subdir == 'glibc' and disable_glibc_option
		continue
	elif test_subdir == 'posix' and disable_posix_option
		continue
	elif test_subdir == 'linux' and disable_linux_option
		continue
	endif

	should_fail = fail_test_cases.contains(test_name)
	exec = executable(test_exec_name, [test_name + '.c', test_sources],
		link_with: libc, include_directories: libc_include_dirs,
		dependencies: rtlib_deps,
		build_rpath: meson.build_root(),
		override_options: test_override_options,
		c_args: test_c_args,
		link_args: test_link_args,
		pie: use_pie,
	)
	test(test_short_name, exec, suite: test_subdir, should_fail: should_fail)

	if build_tests_host_libc and not host_libc_excluded_test_cases.contains(test_name)
		if test_name in host_libc_noasan_test_cases
			host_libc_sanitize_options = 'b_sanitize=undefined'
		else
			host_libc_sanitize_options = 'b_sanitize=address,undefined'
		endif
		exec = executable('host-libc-' + test_exec_name, test_name + '.c',
			build_rpath: meson.build_root(),
			override_options: host_libc_sanitize_options,
			c_args: ['-D_GNU_SOURCE', '-DUSE_HOST_LIBC', '-pthread'],
			link_args: ['-lresolv', '-ldl', '-pthread'],
			native: true,
		)
		test(test_short_name, exec, suite: ['host-libc', test_subdir], should_fail: should_fail)
	endif
endforeach
