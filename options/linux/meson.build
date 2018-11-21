
rtdl_include_dirs += include_directories('include')

libc_include_dirs += include_directories('include')
if disable_linux_option
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

install_headers(
	'include/elf.h',
	'include/getopt.h',
	'include/malloc.h',
	'include/memory.h',
	'include/mntent.h',
	'include/poll.h',
	'include/pty.h',
	'include/stdio_ext.h',
	'include/utmp.h',
	'include/values.h',
)
install_headers(
	'include/asm/ioctls.h',
	subdir: 'asm'
)
install_headers(
	'include/linux/bpf_common.h',
	'include/linux/bsg.h',
	'include/linux/cdrom.h',
	'include/linux/filter.h',
	'include/linux/fs.h',
	'include/linux/hdreg.h',
	'include/linux/input.h',
	'include/linux/kd.h',
	'include/linux/magic.h',
	'include/linux/major.h',
	'include/linux/netlink.h',
	'include/linux/pci_regs.h',
	'include/linux/sched.h',
	'include/linux/sockios.h',
	'include/linux/types.h',
	'include/linux/videodev2.h',
	'include/linux/vt.h',
	subdir: 'linux'
)
install_headers(
	'include/net/if_arp.h',
	subdir: 'net'
)
install_headers(
	'include/scsi/scsi.h',
	'include/scsi/scsi_ioctl.h',
	'include/scsi/sg.h',
	subdir: 'scsi'
)
install_headers(
	'include/sys/epoll.h',
	'include/sys/inotify.h',
	'include/sys/ioctl.h',
	'include/sys/mount.h',
	'include/sys/prctl.h',
	'include/sys/ptrace.h',
	'include/sys/random.h',
	'include/sys/sendfile.h',
	'include/sys/signalfd.h',
	'include/sys/sysmacros.h',
	'include/sys/timerfd.h',
	subdir: 'sys'
)

