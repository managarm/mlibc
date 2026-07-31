sysdep_supported_options = {
	'posix': false,
	'linux': false,
	'glibc': false,
	'bsd': false,
}

if not headers_only
	mlibc_c_args += ['-D_GNU_SOURCE', '-D__MLIBC_WINDOWS']

	dlltool = find_program('dlltool', native: false)
	kernel32_import = custom_target('kernel32-import',
		build_by_default: true,
		command: [dlltool, '--input-def', '@INPUT@', '--output-lib', '@OUTPUT@'],
		input: 'kernel32.def',
		output: 'libkernel32.a',
		install: true,
		install_dir: get_option('libdir')
	)
	synchronization_import = custom_target('synchronization-import',
		build_by_default: true,
		command: [dlltool, '--input-def', '@INPUT@', '--output-lib', '@OUTPUT@'],
		input: 'synchronization.def',
		output: 'libwindows-synchronization.a',
		install: true,
		install_dir: get_option('libdir')
	)

	libc_sources += files(
		'entry.cpp',
		'files.cpp',
		'futex.cpp',
		'libc-crtmain.c',
		'sysdeps.cpp',
		'tcb.cpp',
	)
	libc_include_dirs += include_directories('include')
	rtld_include_dirs += include_directories('include')
	libc_deps += declare_dependency(link_args: [
		kernel32_import.full_path(),
		synchronization_import.full_path(),
	])

	custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('dllcrt1',
		build_by_default: true,
		command: cpp_compiler.cmd_array() + [
			'-fno-stack-protector',
			'-c',
			'-o', '@OUTPUT@', '@INPUT@'
		],
		input: 'dllcrt1.cpp',
		depend_files: files('crt.h'),
		output: 'dllcrt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('mlibc-crtbegin',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crtbegin.S',
		output: 'mlibc-crtbegin.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('mlibc-crtend',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crtend.S',
		output: 'mlibc-crtend.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif

if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/sigevent.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/sched_param.h',
		'include/abi-bits/sigset_t.h',
		subdir: 'abi-bits',
		follow_symlinks: true,
	)
endif
