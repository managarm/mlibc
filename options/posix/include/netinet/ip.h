
#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <netinet/in.h>

#define IPTOS_TOS_MASK 0x1E
#define IPTOS_TOS(tos) ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY 0x10
#define IPTOS_THROUGHPUT 0x08
#define IPTOS_RELIABILITY 0x04
#define IPTOS_LOWCOST 0x02
#define IPTOS_MINCOST IPTOS_LOWCOST
#define IPTOS_CLASS_CS0 0x00
#define IPTOS_CLASS_CS4 0x80
#define IPTOS_CLASS_CS6 0xC0
#define IPTOS_DSCP_EF 0xB8

#define IPOPT_COPY 0x80
#define IPOPT_CLASS_MASK 0x60
#define IPOPT_NUMBER_MASK 0x1f

#define IPOPT_COPIED(o) ((o) & IPOPT_COPY)
#define IPOPT_CLASS(o) ((o) & IPOPT_CLASS_MASK)
#define IPOPT_NUMBER(o) ((o) & IPOPT_NUMBER_MASK)

#define IPOPT_CONTROL 0x00
#define IPOPT_RESERVED1 0x20
#define IPOPT_DEBMEAS 0x40
#define IPOPT_MEASUREMENT IPOPT_DEBMEAS
#define IPOPT_RESERVED2 0x60

#define IPOPT_EOL 0
#define IPOPT_END IPOPT_EOL
#define IPOPT_NOP 1
#define IPOPT_NOOP IPOPT_NOP

#define IPOPT_RR 7
#define IPOPT_TS 68
#define IPOPT_TIMESTAMP IPOPT_TS
#define IPOPT_SECURITY 130
#define IPOPT_SEC IPOPT_SECURITY
#define IPOPT_LSRR 131
#define IPOPT_SATID 136
#define IPOPT_SID IPOPT_SATID
#define IPOPT_SSRR 137
#define IPOPT_RA 148

#define IPOPT_OPTVAL 0
#define IPOPT_OLEN 1
#define IPOPT_OFFSET 2
#define IPOPT_MINOFF 4

#define MAX_IPOPTLEN 40

#define IPOPT_TS_TSONLY 0
#define IPOPT_TS_TSANDADDR 1
#define IPOPT_TS_PRESPEC 3

#define IPDEFTTL 64

struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int ip_hl:4;
	unsigned int ip_v:4;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned int ip_v:4;
	unsigned int ip_hl:4;
#endif
	uint8_t ip_tos;
	unsigned short ip_len;
	unsigned short ip_id;
	unsigned short ip_off;
#define IP_RF 0x8000
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
	uint8_t ip_ttl;
	uint8_t ip_p;
	unsigned short ip_sum;
	struct in_addr ip_src, ip_dst;
};

#define IPVERSION 4

struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int ihl:4;
	unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	unsigned int version:4;
	unsigned int ihl:4;
#else
# error	"Please fix <endian.h>"
#endif
	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
};

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_IP_H */

