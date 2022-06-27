#ifndef _NETPACKET_PACKET_H
#define _NETPACKET_PACKET_H

#include <abi-bits/packet.h>

struct sockaddr_ll {
	unsigned short int sll_family;
	unsigned short int sll_protocol;
	int sll_ifindex;
	unsigned short int sll_hatype;
	unsigned char sll_pkttype;
	unsigned char sll_halen;
	unsigned char sll_addr[8];
};

#endif // _NETPACKET_PACKET_H
