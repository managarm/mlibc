# ANCHOR: sources-and-includes
sysdep_supported_options = {
	'posix': false,
	'linux': false,
	'glibc': false,
	'bsd': false,
}

rtld_sources += files(
	'sysdeps.cpp',
	'syscall.cpp',
)
rtld_include_dirs += include_directories('include')

libc_sources += files(
	'entry.cpp',
	'sysdeps.cpp',
	'syscall.cpp',
)
libc_include_dirs += include_directories('include')
# ANCHOR_END: sources-and-includes

# ANCHOR: abi-includes
if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
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
		'include/abi-bits/riscv-hwprobe.h',
		'include/abi-bits/sigevent.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/sa_family_t.h',
		'include/abi-bits/sockaddr_storage.h',
		'include/abi-bits/sig-limits.h',
		subdir: 'abi-bits',
		follow_symlinks: true
	)
endif
# ANCHOR_END: abi-includes

# ANCHOR: crt1
if not headers_only
	crt = custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
# ANCHOR_END: crt1
