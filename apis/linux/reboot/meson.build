
libc_sources += files(
	'reboot.cpp',
)

if not no_headers
	install_headers(
		'include/sys/reboot.h',
		subdir: 'sys',
	)

	install_headers(
		'../../../sysdeps' / host_machine.system() / 'include/abi-bits/linux/reboot.h',
		subdir: 'abi-bits/linux',
		follow_symlinks: true,
	)
endif

libc_include_dirs += [
	include_directories('include'),
]
