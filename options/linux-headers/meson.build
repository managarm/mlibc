
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
		'include/linux/memfd.h',
		'include/linux/netlink.h',
		'include/linux/pci_regs.h',
		'include/linux/route.h',
		'include/linux/sched.h',
		'include/linux/sockios.h',
		'include/linux/types.h',
		'include/linux/videodev2.h',
		'include/linux/vt.h',
		'include/linux/rtnetlink.h',
		'include/linux/if_ether.h',
		'include/linux/if_infiniband.h',
		'include/linux/if_packet.h',
		'include/linux/genetlink.h',
		'include/linux/neighbour.h',
		'include/linux/keyctl.h',
		'include/linux/if_arp.h',
		'include/linux/if_addrlabel.h',
		'include/linux/if_link.h',
		'include/linux/ip.h',
		'include/linux/if_addr.h',
		'include/linux/if_bridge.h',
		'include/linux/if_tunnel.h',
		'include/linux/l2tp.h',
		'include/linux/pkt_sched.h',
		'include/linux/veth.h',
		subdir: 'linux'
	)
	install_headers(
		'include/scsi/scsi.h',
		'include/scsi/scsi_ioctl.h',
		'include/scsi/sg.h',
		subdir: 'scsi'
	)
	install_headers(
		'include/linux/netfilter/nfnetlink.h',
		'include/linux/netfilter/nfnetlink_compat.h',
		subdir: 'linux/netfilter'
	)
endif
