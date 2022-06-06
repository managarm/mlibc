#ifndef _LINUX_IF_H
#define _LINUX_IF_H

#include <linux/socket.h>

#define IFNAMSIZ 16

#include <linux/hdlc/ioctl.h>

struct ifmap {
	unsigned long mem_start;
	unsigned long mem_end;
	unsigned short base_addr; 
	unsigned char irq;
	unsigned char dma;
	unsigned char port;
};

struct if_settings {
	unsigned int type;
	unsigned int size;
	union {
		raw_hdlc_proto *raw_hdlc;
		cisco_proto *cisco;
		fr_proto *fr;
		fr_proto_pvc *fr_pvc;
		fr_proto_pvc_info *fr_pvc_info;
		x25_hdlc_proto *x25;

		sync_serial_settings *sync;
		te1_settings *te1;
	} ifs_ifsu;
};

struct ifreq {
#define IFHWADDRLEN	6
	union {
		char ifrn_name[IFNAMSIZ];
	} ifr_ifrn;
	
	union {
		struct sockaddr ifru_addr;
		struct sockaddr ifru_dstaddr;
		struct sockaddr ifru_broadaddr;
		struct sockaddr ifru_netmask;
		struct sockaddr ifru_hwaddr;
		short ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct ifmap ifru_map;
		char ifru_slave[IFNAMSIZ];
		char ifru_newname[IFNAMSIZ];
		void *ifru_data;
		struct if_settings ifru_settings;
	} ifr_ifru;
};

#define ifr_name	ifr_ifrn.ifrn_name

#endif
