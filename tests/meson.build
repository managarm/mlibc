ansi_test_cases = [
	'sscanf',
	'sprintf'
]

posix_test_cases = [
	'inet_ntop',
	'inet_pton'
]

if not disable_ansi_option
	foreach f : ansi_test_cases
		exec = executable('ansi-' + f, ['ansi/' + f + '.c', crt],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('ansi/' + f, exec)
	endforeach
endif

if not disable_posix_option
	foreach f : posix_test_cases
		exec = executable('posix-' + f, ['posix/' + f + '.c', crt],
			link_with: libc, include_directories: libc_include_dirs,
			build_rpath: meson.build_root(),
			c_args: '-no-pie',
			link_args: ['-Wl,--dynamic-linker=' + meson.build_root() + '/ld.so',
				'-no-pie'])
		test('posix/' + f, exec)
	endforeach
endif
