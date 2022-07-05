#ifndef  _NET_IF_ARP_H
#define  _NET_IF_ARP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>

#define ARPHRD_ETHER	1
#define ARPHRD_IEEE802  6
#define ARPHRD_SLIP		256

struct arpreq {
	struct sockaddr arp_pa;
	struct sockaddr arp_ha;
	int arp_flags;
	struct sockaddr arp_netmask;
	char arp_dev[16];
};

#endif // _NET_IF_ARP_H

