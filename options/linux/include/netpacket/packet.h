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

struct packet_mreq {
	int mr_ifindex;
	unsigned short int mr_type;
	unsigned short int mr_alen;
	unsigned char mr_address[8];
};

#define PACKET_ADD_MEMBERSHIP 1
#define PACKET_DROP_MEMBERSHIP 2

#define PACKET_MR_MULTICAST 0
#define PACKET_MR_PROMISC 1
#define PACKET_MR_ALLMULTI 2
#define PACKET_MR_UNICAST 3

#endif // _NETPACKET_PACKET_H
