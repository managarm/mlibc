#ifndef __LINUX_IF_PACKET_H
#define __LINUX_IF_PACKET_H

#include <stdint.h>

struct sockaddr_ll {
	unsigned short sll_family;
	uint16_t sll_protocol;
	int sll_ifindex;
	unsigned short sll_hatype;
	unsigned char sll_pkttype;
	unsigned char sll_halen;
	unsigned char sll_addr[8];
};

#endif
