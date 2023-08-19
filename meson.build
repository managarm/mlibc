project('mlibc', default_options: ['warning_level=2', 'cpp_std=c++20'])

fs = import('fs')

rtdl_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/internal' / host_machine.cpu_family() + '-include'),
	include_directories('options/rtdl' / host_machine.cpu_family()),
	include_directories('options/rtdl/include'),
]
libc_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/elf/include'),
	include_directories('options/lsb/include'),
	include_directories('options/rtdl/include'),
	include_directories('options/rtdl' / host_machine.cpu_family()),
	include_directories('options/internal' / host_machine.cpu_family() + '-include')
]

rtdl_sources = [ ]
rtdl_dso_sources = [ ]
libc_sources = [ ]
libc_sublibs = [ ]

libc_deps = [ ]
rtdl_deps = [ ]

headers_only = get_option('headers_only')
no_headers = get_option('mlibc_no_headers')
library_type = get_option('default_library')
build_tests = get_option('build_tests')
build_tests_host_libc = get_option('build_tests_host_libc')
disable_ansi_option = get_option('disable_ansi_option')
disable_posix_option = get_option('disable_posix_option')
disable_linux_option = get_option('disable_linux_option')
disable_iconv_option = get_option('disable_iconv_option')
disable_intl_option = get_option('disable_intl_option')
disable_glibc_option = get_option('disable_glibc_option')
disable_crypt_option = get_option('disable_crypt_option')
disable_bsd_option = get_option('disable_bsd_option')
disable_libgcc_dependency = get_option('disable_libgcc_dependency')
internal_conf = configuration_data()
mlibc_conf = configuration_data()

if not headers_only
	cxxshim_dep = dependency('cxxshim',
				 required: false,
				 fallback: ['cxxshim', 'cxxshim_dep'])
	libc_deps += cxxshim_dep
	rtdl_deps += cxxshim_dep

	frigg_dep = dependency(
		'frigg',
		default_options: ['frigg_no_install=true'],
		fallback: ['frigg', 'frigg_dep'],
	)
	libc_deps += frigg_dep
	rtdl_deps += frigg_dep

	add_languages('c', 'cpp')
	c_compiler = meson.get_compiler('c')
	cpp_compiler = meson.get_compiler('cpp')

	add_project_arguments('-Wno-unused-function', '-D__MLIBC_BUILDING_MLIBC', language: ['c', 'cpp'])
	add_project_arguments('-nostdinc', '-fno-builtin', language: ['c', 'cpp'])
	add_project_arguments('-Werror=misleading-indentation', language: ['c', 'cpp'])
	add_project_arguments('-fno-rtti', '-fno-exceptions', language: 'cpp')
	add_project_link_arguments('-nostdlib', language: ['c', 'cpp'])

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
		rtdl_include_dirs += include_directories(ccdir / 'include-fixed')
		libc_include_dirs += include_directories(ccdir / 'include-fixed')
	endif

	rtdl_include_dirs += include_directories(ccdir / 'include')
	libc_include_dirs += include_directories(ccdir / 'include')

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

	rtdl_include_dirs += cplusplus_include_path
	libc_include_dirs += cplusplus_include_path
endif

internal_conf.set_quoted('MLIBC_SYSTEM_NAME', host_machine.system())
internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', false)
internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', false)
internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', false)
internal_conf.set10('MLIBC_DEBUG_ALLOCATOR', get_option('debug_allocator'))

#----------------------------------------------------------------------------------------
# Configuration based on sysdeps.
#----------------------------------------------------------------------------------------

# Process sysdeps first, as sysdeps might want to disable unsupported options.
provides_bits_syscall_h = false
if host_machine.system() == 'linux'
	provides_bits_syscall_h = true
	rtdl_include_dirs += include_directories('sysdeps/linux/include')
	libc_include_dirs += include_directories('sysdeps/linux/include')

	if not headers_only
		if get_option('linux_kernel_headers') == ''
			error('linux_kernel_headers is not set')
		endif

		if not import('fs').is_dir(get_option('linux_kernel_headers'))
			error('linux_kernel_headers is not set to a valid path')
		endif

		rtdl_include_dirs += include_directories(get_option('linux_kernel_headers'))
		libc_include_dirs += include_directories(get_option('linux_kernel_headers'))
	endif

	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MMAP_ALLOCATE_DSO', true)
	subdir('sysdeps/linux')
elif host_machine.system() == 'aero'
	rtdl_include_dirs += include_directories('sysdeps/aero/include')
	libc_include_dirs += include_directories('sysdeps/aero/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/aero')
elif host_machine.system() == 'managarm'
	# TODO: Adopt the include_directories() commands from the managarm meson.build.
	rtdl_include_dirs += include_directories('sysdeps/managarm/include')
	libc_include_dirs += include_directories('sysdeps/managarm/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/managarm')
elif host_machine.system() == 'ironclad'
	disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/ironclad/include')
	libc_include_dirs += include_directories('sysdeps/ironclad/include')
	subdir('sysdeps/ironclad')
elif host_machine.system() == 'keyronex'
	rtdl_include_dirs += include_directories('sysdeps/keyronex/include')
	libc_include_dirs += include_directories('sysdeps/keyronex/include')
	internal_conf.set10('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set10('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/keyronex')
elif host_machine.system() == 'lyre'
	disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/lyre/include')
	libc_include_dirs += include_directories('sysdeps/lyre/include')
	subdir('sysdeps/lyre')
elif host_machine.system() == 'lemon'
	rtdl_include_dirs += include_directories('sysdeps/lemon/include')
	libc_include_dirs += include_directories('sysdeps/lemon/include')
	subdir('sysdeps/lemon')
elif host_machine.system() == 'dripos'
	disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/dripos/include')
	libc_include_dirs += include_directories('sysdeps/dripos/include')
	subdir('sysdeps/dripos')
else
	error('No sysdeps defined for OS: ' + host_machine.system())
endif

#----------------------------------------------------------------------------------------
# Configuration based on enabled options.
#----------------------------------------------------------------------------------------

mlibc_conf.set10('__MLIBC_ANSI_OPTION', not disable_ansi_option)
mlibc_conf.set10('__MLIBC_POSIX_OPTION', not disable_posix_option)
mlibc_conf.set10('__MLIBC_LINUX_OPTION', not disable_linux_option)
mlibc_conf.set10('__MLIBC_INTL_OPTION', not disable_intl_option)
mlibc_conf.set10('__MLIBC_ICONV_OPTION', not disable_iconv_option)
mlibc_conf.set10('__MLIBC_GLIBC_OPTION', not disable_glibc_option)
mlibc_conf.set10('__MLIBC_CRYPT_OPTION', not disable_crypt_option)
mlibc_conf.set10('__MLIBC_BSD_OPTION', not disable_bsd_option)
mlibc_conf.set10('__MLIBC_SYSDEP_HAS_BITS_SYSCALL_H', provides_bits_syscall_h)

if not disable_ansi_option
	rtdl_include_dirs += include_directories('options/ansi/include')
	libc_include_dirs += include_directories('options/ansi/include')
endif

if not disable_posix_option
	rtdl_include_dirs += include_directories('options/posix/include')
	libc_include_dirs += include_directories('options/posix/include')
endif

if not disable_iconv_option
	rtdl_include_dirs += include_directories('options/iconv/include')
	libc_include_dirs += include_directories('options/iconv/include')
endif

if not disable_intl_option
	libc_include_dirs += include_directories('options/intl/include')
endif

if not disable_linux_option
	if not headers_only
		if get_option('linux_kernel_headers') == ''
			error('linux_kernel_headers is not set')
		endif

		if not import('fs').is_dir(get_option('linux_kernel_headers'))
			error('linux_kernel_headers is not set to a valid path')
		endif
	endif

	rtdl_include_dirs += include_directories('options/linux/include')
	libc_include_dirs += include_directories('options/linux/include')
	rtdl_include_dirs += include_directories(get_option('linux_kernel_headers'))
	libc_include_dirs += include_directories(get_option('linux_kernel_headers'))
endif

if not disable_glibc_option
	rtdl_include_dirs += include_directories('options/glibc/include')
	libc_include_dirs += include_directories('options/glibc/include')
endif

if not disable_crypt_option
	libc_include_dirs += include_directories('options/crypt/include')
endif

if not disable_bsd_option
	rtdl_include_dirs += include_directories('options/bsd/include')
	libc_include_dirs += include_directories('options/bsd/include')
endif

rtdl_include_dirs += include_directories('options/elf/include')
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
	'options/internal/generic/global-config.cpp',
	'options/internal/generic/inline-emitter.cpp',
	'options/internal/generic/locale.cpp',
	'options/internal/generic/sigset.cpp',
	'options/internal/generic/strings.cpp',
	'options/internal/generic/ubsan.cpp',
	'options/internal/generic/threads.cpp',
	'options/internal/gcc/stack_protector.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/gcc/initfini.cpp',
	'options/internal/gcc-extra/cxxabi.cpp',
	'options/internal' / host_machine.cpu_family() / 'setjmp.S',
	'options/internal' / host_machine.cpu_family() / 'fenv.S',
]

internal_dso_sources = [
	'options/internal' / host_machine.cpu_family() / 'mlibc_crtbegin.S',
	'options/internal' / host_machine.cpu_family() / 'mlibc_crtend.S',
]

if not no_headers
	install_headers(
		'options/internal/include/stdint.h'
	)
	install_headers(
		'options/internal/include/bits/wchar_t.h',
		'options/internal/include/bits/wchar.h',
		'options/internal/include/bits/wint_t.h',
		'options/internal/include/bits/size_t.h',
		'options/internal/include/bits/types.h',
		'options/internal/include/bits/ensure.h',
		'options/internal/include/bits/machine.h',
		'options/internal/include/bits/mbstate.h',
		'options/internal/include/bits/nl_item.h',
		'options/internal/include/bits/null.h',
		'options/internal/include/bits/off_t.h',
		'options/internal/include/bits/ssize_t.h',
		'options/internal/include/bits/sigset_t.h',
		'options/internal/include/bits/inline-definition.h',
		'options/internal/include/bits/ether_addr.h',
		'options/internal/include/bits/cpu_set.h',
		'options/internal/include/bits/threads.h',
		'options/internal/include/bits/winsize.h',
		subdir: 'bits'
	)
endif

rtdl_sources += [
	'options/internal/gcc/stack_protector.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/inline-emitter.cpp',
	'options/internal/generic/frigg.cpp',
	'options/internal/generic/ubsan.cpp',
	'options/rtdl/generic/main.cpp',
	'options/rtdl/generic/linker.cpp',
	'options/rtdl' / host_machine.cpu_family() / 'runtime.S'
]

rtdl_dso_sources += ['options/rtdl' / host_machine.cpu_family() / 'entry.S']

subdir('options/elf')
subdir('options/ansi')
subdir('options/posix')
subdir('options/lsb')
subdir('options/glibc')
subdir('options/linux')
subdir('options/iconv')
subdir('options/intl')
subdir('options/crypt')
subdir('options/bsd')

rtlib_deps = []

if not headers_only
	if not disable_libgcc_dependency
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
		'-fno-stack-protector',
		'-DMLIBC_BUILDING_RTDL'
	]

	libc_all_sources = [
		libc_sources,
		internal_sources,
		ansi_sources,
		lsb_sources,
	]

	# Our library have different behaviour when built as static and shared libraries.
	# Hence we need to rebuild the object files with a different define for each mode.
	if library_type in ['static', 'both']
		static_cpp_args = [
			'-DMLIBC_STATIC_BUILD',
			'-DFRIGG_HAVE_LIBC',
		]
		ld_static_lib = static_library('ld', rtdl_sources,
			name_prefix: '',
			cpp_args: ld_cpp_args + static_cpp_args,
			include_directories: rtdl_include_dirs,
			dependencies: rtdl_deps + rtlib_deps,
			install: false
		)
		libc_static = static_library('c', libc_all_sources,
			cpp_args: static_cpp_args + ['-fno-stack-protector'],
			include_directories: libc_include_dirs,
			dependencies: libc_deps + rtlib_deps,
			link_with: [ld_static_lib],
			link_whole: [libc_sublibs, ld_static_lib],
			install: true
		)
	endif
	if library_type in ['shared', 'both']
		ld_shared_lib = shared_library('ld', rtdl_sources + rtdl_dso_sources,
			name_prefix: '',
			cpp_args: ld_cpp_args,
			include_directories: rtdl_include_dirs,
			dependencies: rtdl_deps + rtlib_deps,
			install: true
		)
		libc_shared = shared_library('c', libc_all_sources + internal_dso_sources,
			include_directories: libc_include_dirs,
			dependencies: libc_deps + rtlib_deps,
			link_with: [ld_shared_lib],
			link_whole: libc_sublibs,
			install: true
		)
	endif

	library('pthread', 'dummy-libs/libpthread/src/dummy.cpp', install: true)
	library('rt', 'dummy-libs/librt/src/dummy.cpp', install: true)
	library('util', 'dummy-libs/libutil/src/dummy.cpp', install: true)
	library('m', 'dummy-libs/libm/src/dummy.cpp', install: true)
	if not disable_crypt_option
		library('crypt', 'dummy-libs/libcrypt/src/dummy.cpp', install: true)
	endif
	library('resolv', 'dummy-libs/libresolv/src/dummy.cpp', install: true)
	library('dl', 'dummy-libs/libdl/src/dummy.cpp', install: true)
endif

summary_info = {}
summary_info += {'Build tests': build_tests}
summary_info += {'Build host-libc tests': build_tests_host_libc}
summary(summary_info, bool_yn: true, section: 'tests')

summary_info = {}
summary_info += {'headers-only': headers_only}
summary_info += {'ANSI option': not disable_ansi_option}
summary_info += {'crypt option': not disable_crypt_option}
summary_info += {'POSIX option': not disable_posix_option}
summary_info += {'Linux option': not disable_linux_option}
summary_info += {'iconv option': not disable_iconv_option}
summary_info += {'intl option': not disable_intl_option}
summary_info += {'glibc option': not disable_glibc_option}
summary_info += {'BSD option': not disable_bsd_option}
summary_info += {'debug allocator': get_option('debug_allocator')}
summary_info += {'libgcc dependency': not disable_libgcc_dependency}
summary(summary_info, bool_yn: true, section: 'mlibc options')

if build_tests
	subdir('tests/')
endif

hdoc = find_program('hdoc', required: false)

conf_data = configuration_data()
conf_data.set('source_root', meson.source_root())
conf_data.set('build_root', meson.build_root())
configure_file(input: 'scripts/hdoc.toml.in',
	output: '.hdoc.toml', configuration: conf_data)

if hdoc.found()
	run_target('hdoc', command : [hdoc.full_path(), '--verbose'])
endif
