
if not linux_option
	subdir_done()
endif
libc_sources += files(
	'generic/mntent.cpp',
	'generic/pty.cpp',
	'generic/sys-epoll.cpp',
	'generic/sys-inotify.cpp',
	'generic/sys-mount.cpp',
	'generic/sys-prctl.cpp',
	'generic/sys-ptrace.cpp',
	'generic/sys-random.cpp',
	'generic/sys-sendfile.cpp',
	'generic/sys-signalfd.cpp',
	'generic/sys-timerfd.cpp',
	'generic/sys-eventfd.cpp',
	'generic/sys-reboot.cpp',
	'generic/sys-xattr.cpp',
	'generic/utmp.cpp',
	'generic/utmpx.cpp',
	'generic/linux-unistd.cpp',
	'generic/malloc.cpp',
	'generic/sys-fsuid.cpp',
	'generic/ifaddrs.cpp',
	'generic/sys-sysinfo.cpp',
	'generic/module.cpp',
	'generic/sys-klog.cpp',
	'generic/sched.cpp',
	'generic/sys-quota.cpp',
	'generic/capabilities.cpp',
	'generic/cpuset.cpp',
	'generic/sys-swap.cpp',
	'generic/sys-statfs.cpp',
	'generic/sys-statx.cpp',
	'generic/sys-pidfd.cpp',
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
		'include/bits/linux/linux_stat.h',
		'include/bits/linux/cpu_set.h',
		subdir: 'bits/linux'
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
		'include/sys/quota.h',
		'include/sys/swap.h',
		'include/sys/statfs.h',
		'include/sys/vfs.h',
		'include/sys/pidfd.h',
		subdir: 'sys'
	)
endif

