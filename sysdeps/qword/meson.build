
disable_posix_option = true
disable_linux_option = true

rtdl_include_dirs += include_directories('include')
rtdl_sources += files(
	'generic/file.cpp'
)

libc_include_dirs += include_directories('include')
libc_sources += files(
	'generic/file.cpp'
)

install_headers(
	'include/abi-bits/abi.h',
	'include/abi-bits/auxv.h',
	subdir: 'bits'
)

