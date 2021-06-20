project('mlibc')

fs = import('fs')

rtdl_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/rtdl/include'),
]
libc_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/elf/include'),
	include_directories('options/lsb/include'),
	include_directories('options/glibc/include'),
	include_directories('options/internal' / host_machine.cpu_family() + '-include')
]

rtdl_sources = [ ]
libc_sources = [ ]
libc_sublibs = [ ]

libc_deps = [ ]
rtdl_deps = [ ]

headers_only = get_option('headers_only')
no_headers = get_option('mlibc_no_headers')
static = get_option('static')
build_tests = get_option('build_tests')
disable_ansi_option = get_option('disable_ansi_option')
disable_posix_option = get_option('disable_posix_option')
disable_linux_option = get_option('disable_linux_option')
disable_iconv_option = get_option('disable_iconv_option')
disable_intl_option = get_option('disable_intl_option')
internal_conf = configuration_data()

if not headers_only
	cxxshim_dep = subproject('cxxshim').get_variable('cxxshim_dep')
	libc_deps += cxxshim_dep
	rtdl_deps += cxxshim_dep
	frigg_dep = subproject('frigg',
		default_options: ['frigg_no_install=true']).get_variable('frigg_dep')
	libc_deps += frigg_dep
	rtdl_deps += frigg_dep

	add_languages('c', 'cpp')
	c_compiler = meson.get_compiler('c')

	add_project_arguments('-nostdinc', '-fno-builtin', language: ['c', 'cpp'])
	add_project_arguments('-std=c++20', language: 'cpp')
	add_project_arguments('-fno-rtti', '-fno-exceptions', language: 'cpp')
	add_project_link_arguments('-nostdlib', language: ['c', 'cpp'])

	if c_compiler.get_id() == 'gcc'
		gccdir = run_command('/bin/sh', '-c',
			' '.join(c_compiler.cmd_array())
					+ ' -print-search-dirs | sed -n -e "s/install: \(.*\)/\\1/p"',
			env: ['LANG=C'],
			check: true).stdout().strip()

		rtdl_include_dirs += include_directories(gccdir / 'include')
		libc_include_dirs += include_directories(gccdir / 'include')
		# Ubuntu seems to patch out include-fixed/ (the files are in include/ instead).
		if fs.exists(gccdir / 'include-fixed')
			rtdl_include_dirs += include_directories(gccdir / 'include-fixed')
			libc_include_dirs += include_directories(gccdir / 'include-fixed')
		endif
	elif c_compiler.get_id() == 'clang'
		clangdir = run_command('/bin/sh', '-c',
			' '.join(c_compiler.cmd_array())
					+ ' -print-search-dirs | sed -ne "s/libraries: =\(.*\):.*/\\1/p"',
			env: ['LANG=C'],
			check: true).stdout().strip()

		rtdl_include_dirs += include_directories(clangdir / 'include')
		libc_include_dirs += include_directories(clangdir / 'include')
	endif
endif

internal_conf.set_quoted('MLIBC_SYSTEM_NAME', host_machine.system())
internal_conf.set('MLIBC_MAP_DSO_SEGMENTS', false)
internal_conf.set('MLIBC_MAP_FILE_WINDOWS', false)
internal_conf.set('MLIBC_STATIC_BUILD', static)

#----------------------------------------------------------------------------------------
# Configuration based on sysdeps.
#----------------------------------------------------------------------------------------

# Process sysdeps first, as sysdeps might want to disable unsupported options.
if host_machine.system() == 'linux'
	disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/linux/include')
	libc_include_dirs += include_directories('sysdeps/linux/include')
	subdir('sysdeps/linux')
elif host_machine.system() == 'managarm'
	# TODO: Adopt the include_directories() commands from the managarm meson.build.
	rtdl_include_dirs += include_directories('sysdeps/managarm/include')
	libc_include_dirs += include_directories('sysdeps/managarm/include')
	internal_conf.set('MLIBC_MAP_DSO_SEGMENTS', true)
	internal_conf.set('MLIBC_MAP_FILE_WINDOWS', true)
	subdir('sysdeps/managarm')
elif host_machine.system() == 'qword'
	disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/qword/include')
	libc_include_dirs += include_directories('sysdeps/qword/include')
	subdir('sysdeps/qword')
elif host_machine.system() == 'sigma'
	#disable_linux_option = true
	rtdl_include_dirs += include_directories('sysdeps/sigma/include')
	libc_include_dirs += include_directories('sysdeps/sigma/include')
	libsigma = dependency('sigma')
	libc_deps += libsigma
	rtdl_deps += libsigma
	subdir('sysdeps/sigma')
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

rtdl_include_dirs += include_directories('options/elf/include')
rtdl_include_dirs += include_directories('options/linux/include')
libc_include_dirs += include_directories('options/elf/include')
libc_include_dirs += include_directories('options/linux/include')

#----------------------------------------------------------------------------------------

configure_file(input: 'internal-config.h.in',
	output: 'internal-config.h',
	configuration: internal_conf)

internal_sources = [
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/charcode.cpp',
	'options/internal/generic/charset.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/frigg.cpp',
	'options/internal/generic/sigset.cpp',
	'options/internal/gcc/stack_protector.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/gcc/initfini.cpp',
	'options/internal/gcc-extra/cxxabi.cpp',
	'options/internal' / host_machine.cpu_family() / 'setjmp.S',
	'options/internal' / host_machine.cpu_family() / 'thread.cpp',
]

if not static
	internal_sources += [
		'options/internal/gcc-extra/mlibc_crtbegin.S',
		'options/internal' / host_machine.cpu_family() / 'mlibc_crtend.S',
	]
endif

if not no_headers
	install_headers(
		'options/internal/include/stdint.h'
	)
	install_headers(
		'options/internal/include/bits/wchar_t.h',
		'options/internal/include/bits/wint_t.h',
		'options/internal/include/bits/feature.h',
		'options/internal/include/bits/size_t.h',
		'options/internal/include/bits/types.h',
		'options/internal/include/bits/ensure.h',
		'options/internal/include/bits/machine.h',
		'options/internal/include/bits/mbstate.h',
		'options/internal/include/bits/null.h',
		'options/internal/include/bits/off_t.h',
		'options/internal/include/bits/ssize_t.h',
		'options/internal/include/bits/sigset_t.h',
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
	'options/internal/generic/frigg.cpp',
	'options/rtdl/generic/main.cpp',
	'options/rtdl/generic/linker.cpp',
	'options/rtdl' / host_machine.cpu_family() / 'runtime.S'
]

if not static
	rtdl_sources += 'options/rtdl' / host_machine.cpu_family() / 'entry.S'
endif

subdir('options/elf')
subdir('options/ansi')
subdir('options/posix')
subdir('options/lsb')
subdir('options/glibc')
subdir('options/linux')
subdir('options/iconv')
subdir('options/intl')

if not headers_only
	if not static
		if host_machine.cpu_family() == 'aarch64'
			# Link with libgcc to get the necessary symbols (LSE atomics and math stuff)
			libgcc = meson.get_compiler('c').find_library('gcc')
			libc_deps += libgcc
			rtdl_deps += libgcc
		endif

		ldso_lib = shared_library('ld', rtdl_sources,
				name_prefix: '',
				cpp_args: ['-fvisibility=hidden', '-DMLIBC_BUILDING_RTDL',
					'-fno-stack-protector'],
				include_directories: rtdl_include_dirs,
				dependencies: rtdl_deps,
				install: true)

		libc = shared_library('c',
				[
					libc_sources,
					internal_sources,
					ansi_sources,
					lsb_sources,
				],
				include_directories: libc_include_dirs,
				dependencies: libc_deps,
				link_with: [ldso_lib],
				link_whole: libc_sublibs,
				install: true,
				pic: true)

		shared_library('dl', 'libdl/src/dummy.cpp', install: true)
		shared_library('pthread', 'libpthread/src/dummy.cpp', install: true)
		shared_library('rt', 'librt/src/dummy.cpp', install: true)
		shared_library('util', 'libutil/src/dummy.cpp', install: true)
		shared_library('m', 'libm/src/dummy.cpp', install: true)
		shared_library('crypt', 'libcrypt/src/dummy.cpp', install: true)
		shared_library('resolv', 'libresolv/src/dummy.cpp', install: true)
	else
		ldso_lib = static_library('ld', rtdl_sources,
				name_prefix: '',
				cpp_args: ['-fvisibility=hidden', '-DMLIBC_BUILDING_RTDL', '-DFRIGG_HAVE_LIBC',
				'-fno-stack-protector'],
				include_directories: rtdl_include_dirs,
				dependencies: rtdl_deps,
				install: false)
		libc = static_library('c',
				[
					libc_sources,
					internal_sources,
					ansi_sources,
					lsb_sources,
				],
				cpp_args: ['-DFRIGG_HAVE_LIBC', '-fno-stack-protector'],
				c_args: ['-fno-stack-protector'],
				include_directories: libc_include_dirs,
				dependencies: libc_deps,
				link_with: [ldso_lib],
				link_whole: [libc_sublibs, ldso_lib],
				install: true)

		static_library('pthread', 'libpthread/src/dummy.cpp', install: true)
		static_library('rt', 'librt/src/dummy.cpp', install: true)
		static_library('util', 'libutil/src/dummy.cpp', install: true)
		static_library('m', 'libm/src/dummy.cpp', install: true)
		static_library('crypt', 'libcrypt/src/dummy.cpp', install: true)
		static_library('resolv', 'libresolv/src/dummy.cpp', install: true)
	endif
endif

if build_tests
	subdir('tests/')
endif
