#ifndef _ABIBITS_IN_H
#define _ABIBITS_IN_H

#include <bits/posix/in_addr_t.h>
#include <bits/posix/in_port_t.h>
#include <abi-bits/socket.h>

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
	uint8_t sin_zero[8];
};

struct in6_addr {
	union {
		uint8_t __s6_addr[16];
		uint16_t __s6_addr16[8];
		uint32_t __s6_addr32[4];
	} __in6_union;
};
#define s6_addr __in6_union.__s6_addr
#define s6_addr16 __in6_union.__s6_addr16
#define s6_addr32 __in6_union.__s6_addr32

struct sockaddr_in6 {
	sa_family_t     sin6_family;
	in_port_t       sin6_port;
	uint32_t        sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t        sin6_scope_id;
};

#define MCAST_INCLUDE 1

struct ip_mreq {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

struct ip_mreq_source {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
	struct in_addr imr_sourceaddr;
};

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned        ipv6mr_interface;
};

struct in_pktinfo {
	unsigned int ipi_ifindex;
	struct in_addr ipi_spec_dst;
	struct in_addr ipi_addr;
};

struct in6_pktinfo {
	struct in6_addr ipi6_addr;
	uint32_t ipi6_ifindex;
};

struct group_req {
	uint32_t gr_interface;
	struct sockaddr_storage gr_group;
};

struct group_source_req {
	uint32_t gsr_interface;
	struct sockaddr_storage gsr_group;
	struct sockaddr_storage gsr_source;
};

#ifdef __cplusplus
}
#endif

#define INADDR_ANY        ((in_addr_t) 0x00000000)
#define INADDR_BROADCAST  ((in_addr_t) 0xffffffff)
#define INADDR_NONE       ((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK   ((in_addr_t) 0x7f000001)

#define INADDR_UNSPEC_GROUP     ((in_addr_t) 0xe0000000)
#define INADDR_ALLHOSTS_GROUP   ((in_addr_t) 0xe0000001)
#define INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002)
#define INADDR_ALLSNOOPERS_GROUP ((in_addr_t) 0xe000006a)
#define INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff)

#define IN6ADDR_ANY_INIT      { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46

#define IPPORT_RESERVED 1024

#define IPPROTO_IP       0
#define IPPROTO_HOPOPTS  0
#define IPPROTO_ICMP     1
#define IPPROTO_IGMP     2
#define IPPROTO_IPIP     4
#define IPPROTO_TCP      6
#define IPPROTO_EGP      8
#define IPPROTO_PUP      12
#define IPPROTO_UDP      17
#define IPPROTO_IDP      22
#define IPPROTO_TP       29
#define IPPROTO_DCCP     33
#define IPPROTO_IPV6     41
#define IPPROTO_ROUTING  43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_RSVP     46
#define IPPROTO_GRE      47
#define IPPROTO_ESP      50
#define IPPROTO_AH       51
#define IPPROTO_ICMPV6   58
#define IPPROTO_NONE     59
#define IPPROTO_DSTOPTS  60
#define IPPROTO_MTP      92
#define IPPROTO_BEETPH   94
#define IPPROTO_ENCAP    98
#define IPPROTO_PIM      103
#define IPPROTO_COMP     108
#define IPPROTO_SCTP     132
#define IPPROTO_MH       135
#define IPPROTO_UDPLITE  136
#define IPPROTO_MPLS     137
#define IPPROTO_RAW      255
#define IPPROTO_MAX      256

#define IP_TOS 1
#define IP_TTL 2
#define IP_PKTINFO 8
#define IP_RECVERR 11
#define IP_MULTICAST_IF 32
#define IP_MULTICAST_TTL 33
#define IP_MULTICAST_LOOP 34
#define IP_ADD_MEMBERSHIP 35
#define IP_DROP_MEMBERSHIP 36
#define IP_ADD_SOURCE_MEMBERSHIP 39
#define IP_DROP_SOURCE_MEMBERSHIP 40

#define IPV6_UNICAST_HOPS 16
#define IPV6_MULTICAST_IF 17
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_LOOP 19
#define IPV6_JOIN_GROUP 20
#define IPV6_LEAVE_GROUP 21
#define IPV6_RECVERR 25
#define IPV6_V6ONLY 26
#define IPV6_RECVPKTINFO 49
#define IPV6_PKTINFO 50
#define IPV6_RECVHOPLIMIT 51
#define IPV6_HOPLIMIT 52

#define MCAST_JOIN_SOURCE_GROUP 46
#define MCAST_LEAVE_SOURCE_GROUP 47

/* These defines are needed for compatibility with Linux kernel headers. */
#define __UAPI_DEF_IN_ADDR      0
#define __UAPI_DEF_IN_IPPROTO   0
#define __UAPI_DEF_IN_PKTINFO   0
#define __UAPI_DEF_IP_MREQ      0
#define __UAPI_DEF_SOCKADDR_IN  0
#define __UAPI_DEF_IN_CLASS     0
#define __UAPI_DEF_IN6_ADDR     0
#define __UAPI_DEF_IN6_ADDR_ALT 0
#define __UAPI_DEF_SOCKADDR_IN6 0
#define __UAPI_DEF_IPV6_MREQ    0
#define __UAPI_DEF_IPPROTO_V6   0
#define __UAPI_DEF_IPV6_OPTIONS 0
#define __UAPI_DEF_IN6_PKTINFO  0
#define __UAPI_DEF_IP6_MTUINFO  0

#endif // _ABITBITS_IN_H
