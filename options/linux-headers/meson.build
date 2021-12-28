
if disable_linux_headers
	subdir_done()
endif

if not no_headers
	install_headers(
		'include/asm/ioctl.h',
		'include/asm/ioctls.h',
		subdir: 'asm'
	)
	install_headers(
		'include/linux/bpf_common.h',
		'include/linux/bsg.h',
		'include/linux/cdrom.h',
		'include/linux/fb.h',
		'include/linux/filter.h',
		'include/linux/fs.h',
		'include/linux/hdreg.h',
		'include/linux/input-event-codes.h',
		'include/linux/input.h',
		'include/linux/ioctl.h',
		'include/linux/kd.h',
		'include/linux/magic.h',
		'include/linux/major.h',
		'include/linux/netlink.h',
		'include/linux/pci_regs.h',
		'include/linux/route.h',
		'include/linux/sched.h',
		'include/linux/sockios.h',
		'include/linux/types.h',
		'include/linux/videodev2.h',
		'include/linux/vt.h',
		'include/linux/rtnetlink.h',
		subdir: 'linux'
	)
	install_headers(
		'include/scsi/scsi.h',
		'include/scsi/scsi_ioctl.h',
		'include/scsi/sg.h',
		subdir: 'scsi'
	)
endif
