project('mlibc')

rtdl_include_dirs = [
	include_directories('options/internal/include'),
]
libc_include_dirs = [
	include_directories('options/internal/include'),
	include_directories('options/internal/x86_64-include'),
	include_directories('options/elf/include'),
	include_directories('options/lsb/include'),
	include_directories('options/glibc/include'),

]
rtdl_sources = [ ]
libc_sources = [ ]
libc_sublibs = [ ]

libc_deps = [ ]
rtdl_deps = [ ]

headers_only = get_option('headers_only')
no_headers = get_option('mlibc_no_headers')
static = get_option('static')
disable_ansi_option = false
disable_posix_option = false
disable_linux_option = false
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
	add_project_arguments('-std=c++17', language: 'cpp')
	add_project_arguments('-fno-rtti', '-fno-exceptions', language: 'cpp')
	add_project_link_arguments('-nostdlib', language: ['c', 'cpp'])

	gccdir = run_command('/bin/sh', '-c',
		''.join(c_compiler.cmd_array())
				+ ' -print-search-dirs | sed -n -e "s/install: \(.*\)/\\1/p"',
		check: true).stdout().strip()

	rtdl_include_dirs += include_directories(gccdir + '/include')
	rtdl_include_dirs += include_directories(gccdir + '/include-fixed')
	libc_include_dirs += include_directories(gccdir + '/include')
	libc_include_dirs += include_directories(gccdir + '/include-fixed')
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
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/gcc/initfini.cpp',
	'options/internal/gcc-extra/cxxabi.cpp',
	'options/internal/x86_64/setjmp.S',
	'options/internal/x86_64/thread.cpp',
]

if not static
	internal_sources += [
		'options/internal/gcc-extra/mlibc_crtbegin.S',
		'options/internal/gcc-extra/mlibc_crtend.S',
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
		subdir: 'bits'
	)
endif

rtdl_sources += [
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/frigg.cpp',
	'options/rtdl/generic/main.cpp',
	'options/rtdl/generic/linker.cpp',
	'options/rtdl/x86_64/runtime.S',
]

if not static
	rtdl_sources += 'options/rtdl/x86_64/entry.S'
endif

subdir('options/elf')
subdir('options/ansi')
subdir('options/posix')
subdir('options/lsb')
subdir('options/glibc')
subdir('options/linux')

if not headers_only
	if not static
		ldso_lib = shared_library('ld', rtdl_sources,
				name_prefix: '',
				cpp_args: ['-fvisibility=hidden', '-DMLIBC_BUILDING_RTDL', '-DFRIGG_HAVE_LIBC'],
				include_directories: rtdl_include_dirs,
				dependencies: rtdl_deps,
				install: true)

		shared_library('c',
				[
					libc_sources,
					internal_sources,
					ansi_sources,
					lsb_sources,
				],
				cpp_args: ['-DFRIGG_HAVE_LIBC'],
				include_directories: libc_include_dirs,
				dependencies: libc_deps,
				link_with: [ldso_lib],
				link_whole: libc_sublibs,
				install: true)

		shared_library('dl', 'libdl/src/dummy.cpp', install: true)
		shared_library('pthread', 'libpthread/src/dummy.cpp', install: true)
		shared_library('rt', 'librt/src/dummy.cpp', install: true)
		shared_library('util', 'libutil/src/dummy.cpp', install: true)
		shared_library('m', 'libm/src/dummy.cpp', install: true)
	else
		ldso_lib = static_library('ld', rtdl_sources,
				name_prefix: '',
				cpp_args: ['-fvisibility=hidden', '-DMLIBC_BUILDING_RTDL', '-DFRIGG_HAVE_LIBC'],
				include_directories: rtdl_include_dirs,
				dependencies: rtdl_deps,
				install: false)
		ldso_dep = declare_dependency(link_with: ldso_lib, sources: rtdl_sources)
		static_library('c',
				[
					libc_sources,
					internal_sources,
					ansi_sources,
                                        lsb_sources,
				],
				cpp_args: ['-DFRIGG_HAVE_LIBC'],
				include_directories: libc_include_dirs,
				dependencies: [libc_deps, ldso_dep],
				link_with: [ldso_lib],
				link_whole: libc_sublibs,
				install: true)

		static_library('pthread', 'libpthread/src/dummy.cpp', install: true)
		static_library('rt', 'librt/src/dummy.cpp', install: true)
		static_library('util', 'libutil/src/dummy.cpp', install: true)
		static_library('m', 'libm/src/dummy.cpp', install: true)
	endif
endif
