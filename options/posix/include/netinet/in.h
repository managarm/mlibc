
#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <bits/posix/sa_family_t.h>
#include <bits/posix/in_port_t.h>
#include <bits/posix/in_addr_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct in_addr {
	in_addr_t s_addr;
};

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
};

struct in6_addr {
	uint8_t s6_addr[16];
};

struct sockaddr_in6 {
	sa_family_t sin6_family;
	in_port_t sin6_port;
	uint32_t sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t sin6_scope_id;
};

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned ipv6mr_interface;
};

#define IPPROTO_IP 1
#define IPPROTO_IPV6 2
#define IPPROTO_ICMP 3
#define IPPROTO_RAW 4
#define IPPROTO_TCP 5
#define IPPROTO_UDP 6

#define INADDR_ANY 1
#define INADDR_BROADCAST 2

#define INET_ADDRSTRLEN 1

#define INET6_ADDRSTRELN 1

#define IPV6_JOIN_GROUP 1
#define IPV6_LEAVE_GROUP 2
#define IPV6_MULTICAST_HOPS 3
#define IPV6_MULTICAST_IF 4
#define IPV6_MULTICAST_LOOP 5
#define IPV6_UNICAST_HOPS 6
#define IPV6_V6ONLY 7

#define IN6_IS_ADDR_UNSPECIFIED 1
#define IN6_IS_ADDR_LOOPBACK 2
#define IN6_IS_ADDR_MULTICAST 3
#define IN6_IS_ADDR_LINKLOCAL 4
#define IN6_IS_ADDR_SITELOCAL 5
#define IN6_IS_ADDR_V4MAPPED 6
#define IN6_IS_ADDR_V4COMPAT 7
#define IN6_IS_ADDR_MC_NODELOCAL 8
#define IN6_IS_ADDR_MC_LINKLOCAL 9
#define IN6_IS_ADDR_MC_SITELOCAL 10
#define IN6_IS_ADDR_MC_ORGLOCAL 11
#define IN6_IS_ADDR_MC_GLOBAL 12

#ifdef __cplusplus
}
#endif

#endif // _NETINET_IN_H

