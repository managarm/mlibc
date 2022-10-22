
if disable_linux_option
	subdir_done()
endif
libc_sources += files(
	'generic/mntent-stubs.cpp',
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
	'generic/sys-xattr.cpp',
	'generic/utmp-stubs.cpp',
	'generic/utmpx.cpp',
	'generic/linux-unistd.cpp',
	'generic/malloc.cpp',
	'generic/sys-fsuid.cpp',
	'generic/ifaddrs.cpp',
	'generic/sys-sysinfo.cpp',
	'generic/module.cpp',
	'generic/sys-klog.cpp',
	'generic/sched.cpp',
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
		'include/module.h',
	)
	install_headers(
		'include/bits/linux/linux_unistd.h',
		'include/bits/linux/linux_sched.h',
		subdir: 'bits/linux'
	)
	install_headers(
		'include/net/if_arp.h',
		subdir: 'net'
	)
	install_headers(
		'include/netpacket/packet.h',
		subdir: 'netpacket'
	)
	# libc provides these, not the kernel, so the Linux option shall provide them too.
	install_headers(
		'include/scsi/scsi.h',
		'include/scsi/scsi_ioctl.h',
		'include/scsi/sg.h',
		subdir: 'scsi'
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
		'include/sys/sysmacros.h',
		'include/sys/timerfd.h',
		'include/sys/eventfd.h',
		'include/sys/reboot.h',
		'include/sys/fsuid.h',
		'include/sys/vt.h',
		'include/sys/sysinfo.h',
		'include/sys/klog.h',
		'include/sys/xattr.h',
		subdir: 'sys'
	)
endif

