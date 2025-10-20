project('mlibc', default_options: ['warning_level=2', 'c_std=gnu11', 'cpp_std=c++23'], meson_version: '>=1.3.0')

fs = import('fs')

rtld_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/internal' / host_machine.cpu_family() + '-include'),
	include_directories('options/rtld' / host_machine.cpu_family()),
	include_directories('options/rtld/include'),
]
libc_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/elf/include'),
	include_directories('options/lsb/include'),
	include_directories('options/rtld/include'),
	include_directories('options/rtld' / host_machine.cpu_family()),
	include_directories('options/internal' / host_machine.cpu_family() + '-include')
]

rtld_sources = [ ]
rtld_dso_sources = [ ]
libc_sources = [ ]
libc_sublibs = [ ]

libc_deps = [ ]
rtld_deps = [ ]

headers_only = get_option('headers_only')
no_headers = get_option('no_headers')
library_type = get_option('default_library')
build_tests = get_option('build_tests')
build_tests_host_libc = get_option('build_tests_host_libc')
libgcc_dependency = get_option('libgcc_dependency')
internal_conf = configuration_data()
mlibc_conf = configuration_data()

if not headers_only
	add_languages('c', 'cpp', native: true, required: false)
	add_languages('c', 'cpp', native: false)
	c_compiler = meson.get_compiler('c')
	cpp_compiler = meson.get_compiler('cpp')

	target_triple = run_command(c_compiler.cmd_array(), '-dumpmachine', check: true).stdout()
	use_freestnd_hdrs = get_option('use_freestnd_hdrs').disable_auto_if(
			target_triple.to_lower().contains('mlibc'))

	freestnd_c_hdrs_dep = dependency(
		'freestnd-c-hdrs-' + host_machine.cpu_family(),
		required: use_freestnd_hdrs,
		fallback: ['freestnd-c-hdrs', 'freestnd_c_hdrs_dep'],
	)
	libc_deps += freestnd_c_hdrs_dep
	rtld_deps += freestnd_c_hdrs_dep

	freestnd_cxx_hdrs_dep = dependency(
		'freestnd-cxx-hdrs-' + host_machine.cpu_family(),
		required: use_freestnd_hdrs,
		fallback: ['freestnd-cxx-hdrs', 'freestnd_cxx_hdrs_dep'],
	)
	libc_deps += freestnd_cxx_hdrs_dep
	rtld_deps += freestnd_cxx_hdrs_dep

	frigg_dep = dependency(
		'frigg',
		default_options: [
                  'frigg_no_install=true',
                  'build_tests=disabled'
                ],
		fallback: ['frigg', 'frigg_dep'],
	)
	libc_deps += frigg_dep
	rtld_deps += frigg_dep

	add_project_arguments('-Wno-unused-function', '-D__MLIBC_BUILDING_MLIBC', language: ['c', 'cpp'])
	add_project_arguments('-nostdinc', '-fno-builtin', '-ffreestanding', language: ['c', 'cpp'])
	add_project_arguments('-Werror=misleading-indentation', language: ['c', 'cpp'])
	add_project_arguments('-fno-rtti', '-fno-exceptions', language: 'cpp')
	add_project_link_arguments('-nostdlib', language: ['c', 'cpp'])

	if get_option('buildtype').startswith('debug')
		add_project_arguments('-D__MLIBC_DEBUG', language : ['c', 'cpp'])
	endif

	if not freestnd_c_hdrs_dep.found()
		searchdirs = run_command(c_compiler.cmd_array(), '-print-search-dirs',
					check: true).stdout()
		searchdirs_arr = searchdirs.split('\n')
		searchline = 'install: '
		ccdir = ''
		if c_compiler.get_id() == 'clang'
			searchline = 'libraries: ='
		endif

		foreach line : searchdirs_arr
			if line.startswith(searchline)
				ccdir = line.strip(searchline)
				ccdir = ccdir.split(':')[0]
				break
			endif
		endforeach

		if ccdir == ''
			error('could not find compiler-specific header directory')
		endif

		if c_compiler.get_id() == 'gcc' and fs.exists(ccdir / 'include-fixed')
			rtld_include_dirs += include_directories(ccdir / 'include-fixed')
			libc_include_dirs += include_directories(ccdir / 'include-fixed')
		endif

		rtld_include_dirs += include_directories(ccdir / 'include')
		libc_include_dirs += include_directories(ccdir / 'include')
	endif

	if not freestnd_cxx_hdrs_dep.found()
		cplusplus_include_path = []

		c_output = run_command(c_compiler.cmd_array(), '-E', '-v', '-x', 'c',
					'/dev/null', '-o', '-',
					capture: true,
					check: true).stderr().split('\n')

		cpp_output = run_command(cpp_compiler.cmd_array(), '-E', '-v', '-x',
					'c++', '/dev/null', '-o', '-',
					capture: true,
					check: true).stderr().split('\n')

		c_relevant_lines = []

		relevantmarker = '#include <...>'
		relevant_started = false

		foreach line : c_output
			if relevant_started
				if not line.startswith(' ')
					break
				endif
				c_relevant_lines += line.strip()
			elif line.startswith(relevantmarker)
				relevant_started = true
			endif
		endforeach

		relevant_started = false

		foreach line : cpp_output
			if relevant_started
				if not line.startswith(' ')
					break
				endif
				debug('maybe relevant', line)
				stripped = line.strip()
				if stripped in c_relevant_lines
					debug('not relevant (is C)', line)
					continue
				endif
				cplusplus_include_path += include_directories(stripped)
			elif line.startswith(relevantmarker)
				relevant_started = true
			endif
		endforeach

		rtld_include_dirs += cplusplus_include_path
		libc_include_dirs += cplusplus_include_path
	endif
endif

default_library_paths = get_option('default_library_paths')
if default_library_paths.length() == 0
	target_word_size = {
		'x86_64': 64,
		'x86': 32,
		'riscv64': 64,
		'riscv32': 32,
		'aarch64': 64,
		'arm': 32,
		'm68k': 32,
		'loongarch64': 64,
	}
	if target_word_size.get(target_machine.cpu_family()) == 64
		default_library_paths = ['/lib', '/lib64', '/usr/lib', '/usr/lib64']
	else
		default_library_paths = ['/lib', '/usr/lib']
	endif
endif

internal_conf.set_quoted('MLIBC_SYSTEM_NAME', host_machine.system())
internal_conf.set_quoted('MLIBC_DEFAULT_LIBRARY_PATHS', '\\n'.join(default_library_paths))
internal_conf.set('MLIBC_NUM_DEFAULT_LIBRARY_PATHS', default_library_paths.length())
internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', false)
internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', false)
internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', false)
internal_conf.set10('MLIBC_DEBUG_ALLOCATOR', get_option('debug_allocator'))

#----------------------------------------------------------------------------------------
# Configuration based on sysdeps.
#----------------------------------------------------------------------------------------

rtld_include_dirs += include_directories('sysdeps/generic-helpers/include')
libc_include_dirs += include_directories('sysdeps/generic-helpers/include')

# Process sysdeps first, as sysdeps might want to disable unsupported options.
provides_bits_syscall_h = false
if host_machine.system() == 'linux'
	provides_bits_syscall_h = true
	rtld_include_dirs += include_directories('sysdeps/linux/include')
	libc_include_dirs += include_directories('sysdeps/linux/include')

	if not headers_only
		if get_option('linux_kernel_headers') == ''
			error('linux_kernel_headers is not set')
		endif

		if not import('fs').is_dir(get_option('linux_kernel_headers'))
			error('linux_kernel_headers is not set to a valid path')
		endif

		rtld_include_dirs += include_directories(get_option('linux_kernel_headers'))
		libc_include_dirs += include_directories(get_option('linux_kernel_headers'))
	endif

	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', true)
	subdir('sysdeps/linux')
elif host_machine.system() == 'aero'
	rtld_include_dirs += include_directories('sysdeps/aero/include')
	libc_include_dirs += include_directories('sysdeps/aero/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/aero')
elif host_machine.system() == 'managarm'
	# TODO: Adopt the include_directories() commands from the managarm meson.build.
	rtld_include_dirs += include_directories('sysdeps/managarm/include')
	libc_include_dirs += include_directories('sysdeps/managarm/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/managarm')
elif host_machine.system() == 'ironclad'
	rtld_include_dirs += include_directories('sysdeps/ironclad/include')
	libc_include_dirs += include_directories('sysdeps/ironclad/include')
	internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', true)
	subdir('sysdeps/ironclad')
elif host_machine.system() == 'keyronex'
	rtld_include_dirs += include_directories('sysdeps/keyronex/include')
	libc_include_dirs += include_directories('sysdeps/keyronex/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/keyronex')
elif host_machine.system() == 'vinix'
	rtld_include_dirs += include_directories('sysdeps/vinix/include')
	libc_include_dirs += include_directories('sysdeps/vinix/include')
	subdir('sysdeps/vinix')
elif host_machine.system() == 'lemon'
	rtld_include_dirs += include_directories('sysdeps/lemon/include')
	libc_include_dirs += include_directories('sysdeps/lemon/include')
	subdir('sysdeps/lemon')
elif host_machine.system() == 'dripos'
	rtld_include_dirs += include_directories('sysdeps/dripos/include')
	libc_include_dirs += include_directories('sysdeps/dripos/include')
	subdir('sysdeps/dripos')
elif host_machine.system() == 'astral'
	rtld_include_dirs += include_directories('sysdeps/astral/include')
	libc_include_dirs += include_directories('sysdeps/astral/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', true)
	subdir('sysdeps/astral')
elif host_machine.system() == 'menix'
	rtld_include_dirs += include_directories('sysdeps/menix/include')
	libc_include_dirs += include_directories('sysdeps/menix/include')
	subdir('sysdeps/menix')
elif host_machine.system() == 'nyaux'
	rtld_include_dirs += include_directories('sysdeps/nyaux/include')
	libc_include_dirs += include_directories('sysdeps/nyaux/include')
	subdir('sysdeps/nyaux')
else
	error('No sysdeps defined for OS: ' + host_machine.system())
endif

#----------------------------------------------------------------------------------------
# Configuration based on enabled options.
#----------------------------------------------------------------------------------------

posix_option = get_option('posix_option').require(sysdep_supported_options.get('posix')).allowed()
linux_option = get_option('linux_option').require(sysdep_supported_options.get('linux')).allowed()
glibc_option = get_option('glibc_option').require(sysdep_supported_options.get('glibc')).allowed()
bsd_option = get_option('bsd_option').require(sysdep_supported_options.get('bsd')).allowed()

mlibc_conf.set10('__MLIBC_POSIX_OPTION', posix_option)
mlibc_conf.set10('__MLIBC_LINUX_OPTION', linux_option)
mlibc_conf.set10('__MLIBC_GLIBC_OPTION', glibc_option)
mlibc_conf.set10('__MLIBC_BSD_OPTION', bsd_option)
mlibc_conf.set10('__MLIBC_SYSDEP_HAS_BITS_SYSCALL_H', provides_bits_syscall_h)

rtld_include_dirs += include_directories('options/ansi/include')
libc_include_dirs += include_directories('options/ansi/include')

if posix_option
	rtld_include_dirs += include_directories('options/posix/include')
	libc_include_dirs += include_directories('options/posix/include')
endif

if linux_option
	if not headers_only
		if get_option('linux_kernel_headers') == ''
			error('linux_kernel_headers is not set')
		endif

		if not import('fs').is_dir(get_option('linux_kernel_headers'))
			error('linux_kernel_headers is not set to a valid path')
		endif

		cc_search_path = run_command(meson.get_compiler('cpp', native: true), '-E', '-Wp,-v', '-xc', '/dev/null', check: true).stderr()
		search_paths = []

		search_started = false
		foreach line : cc_search_path.strip().splitlines()
			if not search_started and line == '#include <...> search starts here:'
				search_started = true
				continue
			elif search_started and line == 'End of search list.'
				search_started = false
			elif search_started
				search_paths += line.strip()
			endif
		endforeach

		foreach path : search_paths
			if fs.is_samepath(get_option('linux_kernel_headers'), path)
				warning('Please install Linux kernel headers to a directory using \'make headers_install\'.')
				error('Using system include paths for \'linux_kernel_headers\' is unsupported!')
			endif
		endforeach
	endif

	rtld_include_dirs += include_directories('options/linux/include')
	libc_include_dirs += include_directories('options/linux/include')

	linux_kernel_dep = declare_dependency(include_directories: include_directories(get_option('linux_kernel_headers')))

	libc_deps += linux_kernel_dep
	rtld_deps += linux_kernel_dep
endif

if glibc_option
	rtld_include_dirs += include_directories('options/glibc/include')
	libc_include_dirs += include_directories('options/glibc/include')
endif

if bsd_option
	rtld_include_dirs += include_directories('options/bsd/include')
	libc_include_dirs += include_directories('options/bsd/include')
endif

rtld_include_dirs += include_directories('options/elf/include')
libc_include_dirs += include_directories('options/elf/include')
libc_include_dirs += include_directories('.')

#----------------------------------------------------------------------------------------

configure_file(input: 'internal-config.h.in',
	output: 'internal-config.h',
	configuration: internal_conf)

configure_file(input: 'mlibc-config.h.in',
	output: 'mlibc-config.h',
	configuration: mlibc_conf,
	install: not no_headers,
	install_dir: get_option('includedir'))

internal_sources = [
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/charcode.cpp',
	'options/internal/generic/charset.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/frigg.cpp',
	'options/internal/generic/getopt.cpp',
	'options/internal/generic/global-config.cpp',
	'options/internal/generic/inline-emitter.cpp',
	'options/internal/generic/locale.cpp',
	'options/internal/generic/sigset.cpp',
	'options/internal/generic/strings.cpp',
	'options/internal/generic/ubsan.cpp',
	'options/internal/generic/threads.cpp',
	'options/internal/generic/search.cpp',
	'options/internal/gcc/stack_protector.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/gcc-extra/cxxabi.cpp',
	'options/internal' / host_machine.cpu_family() / 'setjmp.S',
	'options/internal' / host_machine.cpu_family() / 'fenv.S',
]

if host_machine.cpu_family() == 'riscv64'
	internal_sources += [
		'options/internal/riscv64/hwprobe.cpp',
	]
endif

if not no_headers
	install_headers(
		'options/internal/include/stdint.h'
	)

	if host_machine.cpu_family() == 'riscv64'
		install_headers(
			'options/internal/riscv64-include/sys/hwprobe.h',
			subdir: 'sys'
		)
	endif

	install_headers(
		'options/internal/include/bits/wchar_t.h',
		'options/internal/include/bits/wchar.h',
		'options/internal/include/bits/wint_t.h',
		'options/internal/include/bits/wctrans_t.h',
		'options/internal/include/bits/wctype_t.h',
		'options/internal/include/bits/size_t.h',
		'options/internal/include/bits/types.h',
		'options/internal/include/bits/ensure.h',
		'options/internal/include/bits/machine.h',
		'options/internal/include/bits/mbstate.h',
		'options/internal/include/bits/locale.h',
		'options/internal/include/bits/nl_item.h',
		'options/internal/include/bits/null.h',
		'options/internal/include/bits/off_t.h',
		'options/internal/include/bits/file.h',
		'options/internal/include/bits/ssize_t.h',
		'options/internal/include/bits/sigset_t.h',
		'options/internal/include/bits/inline-definition.h',
		'options/internal/include/bits/ether_addr.h',
		'options/internal/include/bits/cpu_set.h',
		'options/internal/include/bits/threads.h',
		'options/internal/include/bits/winsize.h',
		'options/internal/include/bits/search.h',
		'options/internal/include/bits/getopt.h',
		subdir: 'bits'
	)
endif

rtld_sources += [
	'options/internal/gcc/stack_protector.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/inline-emitter.cpp',
	'options/internal/generic/frigg.cpp',
	'options/internal/generic/ubsan.cpp',
	'options/rtld/generic/main.cpp',
	'options/rtld/generic/linker.cpp',
	'options/rtld' / host_machine.cpu_family() / 'runtime.S'
]

rtld_dso_sources += ['options/rtld' / host_machine.cpu_family() / 'entry.S']

subdir('options/elf')
subdir('options/ansi')
subdir('options/posix')
subdir('options/lsb')
subdir('options/glibc')
subdir('options/linux')
subdir('options/bsd')

rtlib_deps = []

if not headers_only
	if libgcc_dependency
		libgcc = meson.get_compiler('c').find_library('gcc', required: false)

		compiler_rt_name = 'libclang_rt.builtins-' + host_machine.cpu_family()
		compiler_rt = meson.get_compiler('c').find_library(compiler_rt_name, required: false)

		if not compiler_rt.found()
			compiler_rt_name = 'libclang_rt.builtins'
			compiler_rt = meson.get_compiler('c').find_library(compiler_rt_name, required: false)
		endif

		if libgcc.found()
			rtlib_deps += libgcc
		elif compiler_rt.found()
			rtlib_deps += compiler_rt
		else
			error('neither libgcc nor ' + compiler_rt_name + ' was found')
		endif
	endif

	ld_cpp_args = [
		'-fvisibility=hidden',
		'-fvisibility-inlines-hidden',
		'-fno-stack-protector',
		'-DMLIBC_BUILDING_RTLD',
		'-Wno-extern-c-compat',
		'-Wno-unknown-pragmas',
	]

	libc_cpp_args = [
		'-Wno-unknown-pragmas',
		'-DFRG_HAVE_LIBC',
	]

	if c_compiler.get_id() == 'clang'
		libc_cpp_args += ['-fno-sanitize=function', '-Wno-vla-cxx-extension']
		ld_cpp_args += ['-fno-sanitize=function']
	endif

	libc_all_sources = [
		libc_sources,
		internal_sources,
		ansi_sources,
		lsb_sources,
	]

	ld_library_name = get_option('ld_library_name')

	# Our libraries have different behaviour when built as static and shared libraries.
	# Hence we need to rebuild the object files with a different define for each mode.
	if library_type in ['static', 'both']
		static_cpp_args = [
			'-DMLIBC_STATIC_BUILD',
		]
		ld_static_lib = static_library(ld_library_name, rtld_sources,
			name_prefix: '',
			cpp_args: ld_cpp_args + static_cpp_args,
			include_directories: rtld_include_dirs,
			dependencies: rtld_deps + rtlib_deps,
			install: false
		)
		libc_static = static_library('c', libc_all_sources,
			cpp_args: static_cpp_args + ['-fno-stack-protector', libc_cpp_args],
			include_directories: libc_include_dirs,
			dependencies: libc_deps + rtlib_deps,
			link_with: [ld_static_lib],
			link_whole: [libc_sublibs, ld_static_lib],
			install: true
		)
	endif
	if library_type in ['shared', 'both']
		ld_shared_lib = shared_library(ld_library_name, rtld_sources + rtld_dso_sources,
			name_prefix: '',
			cpp_args: ld_cpp_args,
			include_directories: rtld_include_dirs,
			dependencies: rtld_deps + rtlib_deps,
			install: true
		)
		hide_everything_ld = (meson.current_source_dir()
				      / 'scripts/hide-everything.ld')
		libc_shared = shared_library('c', libc_all_sources,
			cpp_args: [libc_cpp_args],
			include_directories: libc_include_dirs,
			dependencies: libc_deps + rtlib_deps,
			link_with: [ld_shared_lib],
			link_whole: libc_sublibs,
			link_args: ['-Wl,--version-script,' + hide_everything_ld],
			link_depends: [hide_everything_ld],
			install: true
		)
	endif

	library('pthread', 'dummy-libs/libpthread/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('rt', 'dummy-libs/librt/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('util', 'dummy-libs/libutil/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('m', 'dummy-libs/libm/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('resolv', 'dummy-libs/libresolv/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('dl', 'dummy-libs/libdl/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('ssp', 'dummy-libs/libssp/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
	library('ssp_nonshared', 'dummy-libs/libssp_nonshared/src/dummy.cpp', cpp_args: libc_cpp_args, install: true)
endif

summary_info = {}
summary_info += {'Build tests': build_tests}
summary_info += {'Build host-libc tests': build_tests_host_libc}
summary(summary_info, bool_yn: true, section: 'tests')

summary_info = {}
summary_info += {'headers-only': headers_only}
summary_info += {'POSIX option': get_option('posix_option')}
summary_info += {'Linux option': get_option('linux_option')}
summary_info += {'glibc option': get_option('glibc_option')}
summary_info += {'BSD option': get_option('bsd_option')}
summary_info += {'debug allocator': get_option('debug_allocator')}
summary_info += {'libgcc dependency': libgcc_dependency}
summary(summary_info, bool_yn: true, section: 'mlibc options')

if build_tests
	subdir('tests/')
endif

hdoc = find_program('hdoc', required: false)

conf_data = configuration_data()
conf_data.set('source_root', meson.global_source_root())
conf_data.set('build_root', meson.global_build_root())
configure_file(input: 'scripts/hdoc.toml.in',
	output: '.hdoc.toml', configuration: conf_data)

if hdoc.found()
	run_target('hdoc', command : [hdoc.full_path(), '--verbose'])
endif
