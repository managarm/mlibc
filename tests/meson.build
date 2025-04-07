timeout_sec = 10

all_test_cases = [
	'ansi/alloc',
	'ansi/sscanf',
	'ansi/sprintf',
	'ansi/snprintf',
	'ansi/utf8',
	'ansi/strtol',
	'ansi/strtof',
	'ansi/abs',
	'ansi/longjmp',
	'ansi/mbrtoc32',
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
	'ansi/creal-cimag',
	'ansi/fenv',
	'ansi/qsort',
	'ansi/freopen',
	'ansi/strxfrm',
	'ansi/calloc',
	'ansi/fgetpos',
	'ansi/fputs',
	'ansi/ftell',
	'bsd/ns_get_put',
	'bsd/reallocarray',
	'bsd/strl',
	'bsd/sbrk',
	'bsd/getloadavg',
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
	'posix/pthread_schedparam',
	'posix/pwd',
	'posix/fdopen',
	'posix/fopencookie',
	'posix/fmemopen',
	'posix/getaddrinfo',
	'posix/getdelim',
	'posix/getnameinfo',
	'posix/getparam',
	'posix/getservbyname',
	'posix/getservbyport',
	'posix/grp',
	'posix/dprintf',
	'posix/posix_memalign',
	'posix/posix_spawn',
	'posix/index',
	'posix/rindex',
	'posix/search',
	'posix/semaphore',
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
	'posix/sigtimedwait',
	'posix/if_indextoname',
	'posix/readv-writev',
	'posix/posix-timer',
	'posix/strdupa',
	'posix/mkstemp',
	'posix/waitid',
	'posix/usershell',
	'posix/shm',
	'posix/swab',
	'glibc/getopt',
	'glibc/ffsl-ffsll',
	'glibc/error_message_count',
	'glibc/error_one_per_line',
	'glibc/error_print_progname',
	'glibc/error_expect_fail',
	'glibc/error',
	'glibc/error_at_line',
	'glibc/getgrouplist',
	'glibc/rpmatch',
	'linux/xattr',
	'linux/pthread_setname_np',
	'linux/pthread_attr',
	'linux/cpuset',
	'linux/malloc-usable-size',
	'linux/getifaddrs',
	'linux/pidfd',
	'linux/timerfd',
]

if host_machine.system() == 'linux'
	all_test_cases += 'glibc/linux-syscall'
endif

fail_test_cases = [
	'posix/abort',
	'glibc/error_expect_fail',
]

wrapped_test_cases = [
	'glibc/error',
	'glibc/error_at_line',
]

host_libc_excluded_test_cases = [
	'bsd/strl', # These functions do not exist on Linux.
]
host_libc_noasan_test_cases = [
	'posix/pthread_cancel',
	'posix/pthread_attr', # does some stack overflowing to check stack size
	'posix/posix_memalign',
	'posix/search', # requires tdelete (#351)
	'ansi/calloc', # does some overflowing
	'linux/pthread_attr', # encounters memory leaks
]

extra_cflags_test_cases = {
	'ansi/calloc': ['-Wno-alloc-size-larger-than'],
}

test_sources = []
test_objects = []
test_link_args = []
test_c_args = ['-D_GNU_SOURCE', '-U__MLIBC_DEBUG', '-U__MLIBC_BUILDING_MLIBC', '-fhosted', '-fno-builtin']
use_pie = false

host_test_c_args = ['-fno-builtin']

c_compiler = meson.get_compiler('c')

if c_compiler.get_id() == 'gcc'
	test_c_args += []
	host_test_c_args += []
elif c_compiler.get_id() == 'clang'
	test_c_args += ['-Wno-unknown-warning-option']
	host_test_c_args += ['-Wno-unknown-warning-option']
endif

# Our ubsan implementation can't be used by the tests themselves,
# since it is internal to libc.so and ld.so.
test_override_options = ['b_sanitize=none']

if library_type == 'static'
	c_compiler = meson.get_compiler('c')
	searchdirs = run_command(c_compiler.cmd_array(), '-print-search-dirs',
				check: true).stdout()
	searchdirs_arr = searchdirs.split('\n')
	searchline = 'libraries: ='
	searchpaths = ''

	foreach line : searchdirs_arr
		if line.startswith(searchline)
			searchpaths = line.strip(searchline)
			break
		endif
	endforeach

	if searchpaths == ''
		error('could not find compiler-specific library directory')
	endif

	searchpaths_arr = searchpaths.split(':')
	crtpath = ''
	fs = import('fs')
	foreach path : searchpaths_arr
		if fs.exists(path / 'crtbegin.o')
			crtpath = path
			break
		endif
	endforeach

	if crtpath == ''
		error('could not find crtbegin.o/crtend.o')
	endif
	test_objects += [crtpath / 'crtbegin.o', crtpath / 'crtend.o']

	libc_dep = declare_dependency(
		include_directories: libc_include_dirs,
		link_with: libc_static,
		dependencies: [libc_deps, rtlib_deps]
	)
	use_pie = false
	test_c_args += '-no-pie'
	test_link_args += ['-no-pie', '-static']
	test_sources += crt
else
	libc_dep = declare_dependency(
		include_directories: libc_include_dirs,
		link_with: libc_shared,
		dependencies: [libc_deps, rtlib_deps]
	)
	test_link_args += ['-Wl,--dynamic-linker=' + meson.global_build_root() + '/ld.so']

	if host_machine.system() in ['linux']
		use_pie = true
		test_sources += crt_pie
	else
		use_pie = false
		test_sources += crt

		# Meson doesn't set these for us (issue #4651).
		test_c_args += '-no-pie'
		test_link_args += '-no-pie'
	endif

	# Add the rtld tests.
	if posix_option
		subdir('rtld')
	endif
endif

py = import('python').find_installation('python3')

foreach test_name : all_test_cases
	test_subdir = test_name.split('/')[0]
	test_short_name = test_name.split('/')[1]
	test_exec_name = test_name.replace('/', '-')

	if test_subdir == 'bsd' and not bsd_option
		continue
	elif test_subdir == 'glibc' and not glibc_option
		continue
	elif test_subdir == 'posix' and not posix_option
		continue
	elif test_subdir == 'linux' and not linux_option
		continue
	endif

	should_fail = fail_test_cases.contains(test_name)
	exec = executable(test_exec_name, [test_name + '.c', test_sources],
		dependencies: libc_dep,
		objects: test_objects,
		build_rpath: meson.global_build_root(),
		override_options: test_override_options,
		c_args: test_c_args,
		link_args: test_link_args,
		pie: use_pie,
	)

	if wrapped_test_cases.contains(test_name)
		test(test_short_name,
			py,
			suite: test_subdir,
			should_fail: should_fail,
			args: [
				meson.global_source_root() + '/tests/' + test_name + '.py',
				exec
			],
			timeout: timeout_sec,
		)
	else
		test(test_short_name, exec, suite: test_subdir, should_fail: should_fail, timeout: timeout_sec)
	endif

	if build_tests_host_libc and not host_libc_excluded_test_cases.contains(test_name)
		if test_name in host_libc_noasan_test_cases
			host_libc_sanitize_options = 'b_sanitize=undefined'
		else
			host_libc_sanitize_options = 'b_sanitize=address,undefined'
		endif

		if test_name in extra_cflags_test_cases
			extra_cflags = extra_cflags_test_cases[test_name]
		else
			extra_cflags = []
		endif

		exec = executable('host-libc-' + test_exec_name, test_name + '.c',
			build_rpath: meson.global_build_root(),
			override_options: host_libc_sanitize_options,
			c_args: [host_test_c_args + '-D_GNU_SOURCE', '-DUSE_HOST_LIBC', '-pthread', extra_cflags],
			link_args: ['-lresolv', '-ldl', '-pthread', '-lm', '-lrt'],
			native: true,
		)
		test(test_short_name, exec, suite: ['host-libc', test_subdir], should_fail: should_fail, timeout: timeout_sec)
	endif
endforeach
