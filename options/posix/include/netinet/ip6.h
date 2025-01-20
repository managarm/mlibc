#ifndef _NETINET_IP6_H
#define _NETINET_IP6_H

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ip6_hdr {
	union {
		struct ip6_hdrctl {
			uint32_t ip6_un1_flow;
			uint16_t ip6_un1_plen;
			uint8_t  ip6_un1_nxt;
			uint8_t  ip6_un1_hlim;
		} ip6_un1;
		uint8_t ip6_un2_vfc;
	} ip6_ctlun;
	struct in6_addr ip6_src;
	struct in6_addr ip6_dst;
};

#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_IP6_H */
