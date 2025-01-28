#ifndef _NET_ROUTE_H
#define _NET_ROUTE_H

#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTF_HOST 0x0004
#define RTF_REJECT 0x0200

struct rtentry {
	unsigned long int rt_pad1;
	struct sockaddr rt_dst;
	struct sockaddr rt_gateway;
	struct sockaddr rt_genmask;
	unsigned short int rt_flags;
	short int rt_pad2;
	unsigned long int rt_pad3;
	unsigned char rt_tos;
	unsigned char rt_class;
	short int rt_pad4[3];
	short int rt_metric;
	char *rt_dev;
	unsigned long int rt_mtu;
	unsigned long int rt_window;
	unsigned short int rt_irtt;
};

#ifdef __cplusplus
}
#endif

#endif /* _NET_ROUTE_H */
