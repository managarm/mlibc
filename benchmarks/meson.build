if meson.is_cross_build()
	subdir_done()
endif

benchmark_dep = subproject('google-benchmark', default_options: ['tests=disabled', 'default_library=static'], required: true).get_variable('google_benchmark_main_dep')

all_benchmarks = [
	'ansi/strcmp'
]

# We link with libc_static if available, otherwise libc_shared.
# This compiles the benchmark as a standard hosted C++ program, avoiding
# header resolution and runtime linker conflicts with the host C++ standard library.
if library_type == 'static' or library_type == 'both'
	benchmark_link_libs = [libc_static]
else
	benchmark_link_libs = [libc_shared]
endif

benchmark_utils = files(
	'utils.cpp',
)

foreach name : all_benchmarks
	subdir = name.split('/')[0]
	short_name = name.split('/')[1]
	exec_name = name.replace('/', '-')

	if subdir == 'bsd' and not bsd_option
		continue
	elif subdir == 'glibc' and not glibc_option
		continue
	elif subdir == 'posix' and not posix_option
		continue
	elif subdir == 'linux' and not linux_option
		continue
	elif subdir == 'linux-wrappers' and not linux_wrappers_option
		continue
	endif

	extra_cflags = []

	benchmark_cpp_args = []
	foreach arg : mlibc_c_args
		if arg != '-ffreestanding'
			benchmark_cpp_args += arg
		endif
	endforeach

	exec = executable(exec_name, files(name + '.cpp') + benchmark_utils,
		dependencies: benchmark_dep,
		link_with: benchmark_link_libs,
		build_rpath: meson.global_build_root(),
		cpp_args: benchmark_cpp_args + extra_cflags + ['-fexceptions', '-frtti'],
		build_by_default: false,
	)

	benchmark(short_name, exec, suite: subdir, timeout: 180)

	if build_tests_host_libc
		host_libc_sanitize_options = 'b_sanitize=address,undefined'

		exec = executable('host-libc-' + exec_name, files(name + '.cpp') + benchmark_utils,
			build_rpath: meson.global_build_root(),
			override_options: host_libc_sanitize_options,
			cpp_args: [host_test_c_args + '-D_GNU_SOURCE', '-DUSE_HOST_LIBC', '-pthread', '-fno-builtin', extra_cflags, '-fexceptions', '-frtti'],
			link_args: ['-lresolv', '-ldl', '-pthread', '-lm', '-lrt'],
			dependencies: benchmark_dep,
			native: true,
			build_by_default: false,
		)

		benchmark(short_name, exec, suite: ['host-libc', subdir], env: host_env, timeout: 180)
	endif
endforeach
