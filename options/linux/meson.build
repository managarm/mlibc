
if disable_linux_option
	subdir_done()
endif
libc_sources += files(
	'generic/mntent-stubs.cpp',
	'generic/poll-stubs.cpp',
	'generic/pty-stubs.cpp',
	'generic/sys-epoll.cpp',
	'generic/sys-inotify-stubs.cpp',
	'generic/sys-mount.cpp',
	'generic/sys-prctl-stubs.cpp',
	'generic/sys-ptrace-stubs.cpp',
	'generic/sys-random-stubs.cpp',
	'generic/sys-sendfile-stubs.cpp',
	'generic/sys-signalfd.cpp',
	'generic/sys-timerfd.cpp',
	'generic/sys-eventfd.cpp',
	'generic/sys-reboot.cpp',
	'generic/netinet-in6addr.cpp',
	'generic/utmp-stubs.cpp',
	'generic/utmpx.cpp',
	'generic/linux-unistd.cpp',
	'generic/malloc.cpp',
)

if not no_headers
	install_headers(
		'include/ifaddrs.h',
		'include/malloc.h',
		'include/memory.h',
		'include/mntent.h',
		'include/pty.h',
		'include/utmp.h',
		'include/utmpx.h',
		'include/values.h',
		'include/lastlog.h',
	)
	install_headers(
		'include/bits/linux/linux_unistd.h',
		subdir: 'bits/linux'
	)
	install_headers(
		'include/net/if_arp.h',
		subdir: 'net'
	)
	install_headers(
		'include/sys/epoll.h',
		'include/sys/inotify.h',
		'include/sys/mount.h',
		'include/sys/prctl.h',
		'include/sys/ptrace.h',
		'include/sys/random.h',
		'include/sys/sendfile.h',
		'include/sys/signalfd.h',
		'include/sys/syscall.h',
		'include/sys/sysmacros.h',
		'include/sys/timerfd.h',
		'include/sys/eventfd.h',
		'include/sys/reboot.h',
		subdir: 'sys'
	)
endif

