project('mlibc', 'c', 'cpp')

cxxshim_dep = subproject('cxxshim').get_variable('cxxshim_dep')
frigg_dep = subproject('frigg').get_variable('frigg_dep')

rtdl_include_dirs = [ ]
libc_include_dirs = [ ]
rtdl_sources = [ ]
libc_sources = [ ]

disable_posix_option = false
disable_linux_option = false

# Process sysdeps first, as sysdeps might want to disable unsupported options.
if host_machine.system() == 'managarm'
	subdir('sysdeps/managarm')
elif host_machine.system() == 'qword'
	subdir('sysdeps/qword')
else
	error('No sysdeps defined for OS: ' + host_machine.system())
endif

internal_incl = include_directories('options/internal/include')
internal_sources = [
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/frigg.cpp',
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/gcc/initfini.cpp',
	'options/internal/gcc-extra/mlibc_crtbegin.S',
	'options/internal/gcc-extra/mlibc_crtend.S',
	'options/internal/x86_64/setjmp.S',
]
install_headers(
	'options/internal/include/stdint.h'
)
install_headers(
	'options/internal/include/bits/wchar_t.h',
	'options/internal/include/bits/feature.h',
	'options/internal/include/bits/size_t.h',
	'options/internal/include/bits/types.h',
	'options/internal/include/bits/ensure.h',
	'options/internal/include/bits/machine.h',
	'options/internal/include/bits/null.h',
	subdir: 'bits'
)

rtdl_sources += [
	'options/internal/gcc/guard-abi.cpp',
	'options/internal/generic/allocator.cpp',
	'options/internal/generic/debug.cpp',
	'options/internal/generic/ensure.cpp',
	'options/internal/generic/essential.cpp',
	'options/internal/generic/frigg.cpp',
	'options/rtdl/generic/main.cpp',
	'options/rtdl/generic/linker.cpp',
	'options/rtdl/x86_64/runtime.S'
]

subdir('options/ansi')
subdir('options/posix')
subdir('options/lsb')
subdir('options/linux')

c_compiler = meson.get_compiler('c')
gccdir = run_command('/bin/sh', '-c',
	''.join(c_compiler.cmd_array()) + ' -print-search-dirs | sed -n -e "s/install: \(.*\)/\\1/p"',
	check: true).stdout().strip()

add_project_arguments('-nostdinc', '-fno-builtin', language: ['c', 'cpp'])
add_project_arguments('-std=c++17', language: 'cpp')
add_project_arguments('-fno-rtti', '-fno-exceptions', language: 'cpp')
add_project_link_arguments('-nostdlib', language: ['c', 'cpp'])

ldso_lib = shared_library('ld', rtdl_sources,
		name_prefix: '',
		cpp_args: ['-fvisibility=hidden', '-DMLIBC_BUILDING_RTDL', '-DFRIGG_HAVE_LIBC'],
		include_directories: [
			rtdl_include_dirs,
			internal_incl,
			ansi_incl,
			include_directories(gccdir + '/include'),
		],
		dependencies: [cxxshim_dep, frigg_dep],
		install: true)

shared_library('c',
		[
			libc_sources,
			internal_sources,
			ansi_sources,
			lsb_sources,
		],
		cpp_args: ['-DFRIGG_HAVE_LIBC'],
		include_directories: [
			libc_include_dirs,
			internal_incl,
			ansi_incl,
			lsb_incl,
			include_directories(gccdir + '/include'),
		],
		dependencies: [cxxshim_dep, frigg_dep],
		link_with: [ldso_lib],
		install: true)

