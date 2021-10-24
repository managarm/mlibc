#ifndef _LINUX_ROUTE_H
#define _LINUX_ROUTE_H

#include <net/if.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rtentry {
	unsigned long rt_pad1;
	struct sockaddr rt_dst;
	struct sockaddr rt_gateway;
	struct sockaddr rt_genmask;
	unsigned short rt_flags;
	short rt_pad2;
	unsigned long rt_pad3;
	void *rt_pad4;
	short rt_metric;
	char *rt_dev;
	unsigned long rt_mtu;
	unsigned long rt_window;
	unsigned short rt_irtt;
};

#define RTF_UP 0x0001
#define RTF_GATEWAY 0x0002
#define RTF_HOST 0x0004
#define RTF_REINSTATE 0x0008
#define RTF_DYNAMIC 0x0010
#define RTF_MODIFIED 0x0020
#define RTF_MTU 0x0040
#define RTF_MSS RTF_MTU
#define RTF_WINDOW 0x0080
#define RTF_IRTT 0x0100
#define RTF_REJECT 0x0200

#ifdef __cplusplus
}
#endif

#endif // _LINUX_ROUTE_H
