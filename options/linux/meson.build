
rtdl_include_dirs += include_directories('include')

libc_include_dirs += include_directories('include')
if disable_posix_option
	subdir_done()
endif
libc_sources += files(
	'generic/getopt-stubs.cpp',
	'generic/mntent-stubs.cpp',
	'generic/poll-stubs.cpp',
	'generic/pty-stubs.cpp',
	'generic/stdio_ext-stubs.cpp',
	'generic/sys-epoll.cpp',
	'generic/sys-inotify-stubs.cpp',
	'generic/sys-ioctl.cpp',
	'generic/sys-mount.cpp',
	'generic/sys-prctl-stubs.cpp',
	'generic/sys-ptrace-stubs.cpp',
	'generic/sys-random-stubs.cpp',
	'generic/sys-sendfile-stubs.cpp',
	'generic/sys-signalfd.cpp',
	'generic/sys-timerfd.cpp',
)

