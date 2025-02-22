#ifndef _NETINET_ICMP6_H
#define _NETINET_ICMP6_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <abi-bits/in.h>
#include <mlibc-config.h>

#if __MLIBC_GLIBC_OPTION
#include <bits/glibc/glibc_icmp6.h>
#endif /* __MLIBC_GLIBC_OPTION */

#define ICMP6_FILTER 1

#define ICMP6_DST_UNREACH 1
#define ICMP6_PACKET_TOO_BIG 2
#define ICMP6_TIME_EXCEEDED 3
#define ICMP6_PARAM_PROB 4

#define ICMP6_FILTER_BLOCK 1
#define ICMP6_FILTER_PASS 2
#define ICMP6_FILTER_BLOCKOTHERS 3
#define ICMP6_FILTER_PASSONLY 4
#define ICMP6_ECHO_REQUEST 128
#define ICMP6_ECHO_REPLY 129

#define MLD_LISTENER_QUERY 130
#define MLD_LISTENER_REPORT 131
#define MLD_LISTENER_REDUCTION 132

#define ICMP6_DST_UNREACH_NOROUTE 0
#define ICMP6_DST_UNREACH_ADMIN 1
#define ICMP6_DST_UNREACH_BEYONDSCOPE 2
#define ICMP6_DST_UNREACH_ADDR 3
#define ICMP6_DST_UNREACH_NOPORT 4

#define ICMP6_TIME_EXCEED_TRANSIT 0
#define ICMP6_TIME_EXCEED_REASSEMBLY 1

#define ICMP6_PARAMPROB_HEADER 0
#define ICMP6_PARAMPROB_NEXTHEADER 1
#define ICMP6_PARAMPROB_OPTION 2

struct icmp6_filter {
	uint32_t icmp6_filt[8];
};

struct icmp6_hdr {
	uint8_t icmp6_type;
	uint8_t icmp6_code;
	uint16_t icmp6_cksum;
	union {
		uint32_t icmp6_un_data32[1];
		uint16_t icmp6_un_data16[2];
		uint8_t icmp6_un_data8[4];
	} icmp6_dataun;
};

#define icmp6_data32 icmp6_dataun.icmp6_un_data32
#define icmp6_data16 icmp6_dataun.icmp6_un_data16
#define icmp6_data8 icmp6_dataun.icmp6_un_data8

#define icmp6_pptr icmp6_data32[0]
#define icmp6_mtu icmp6_data32[0]
#define icmp6_id icmp6_data16[0]
#define icmp6_seq icmp6_data16[1]
#define icmp6_maxdelay icmp6_data16[0]

#define ICMP6_FILTER_WILLPASS(type, filterp) \
	((((filterp)->icmp6_filt[(type) >> 5]) & (1U << ((type) & 31))) == 0)

#define ICMP6_FILTER_WILLBLOCK(type, filterp) \
	((((filterp)->icmp6_filt[(type) >> 5]) & (1U << ((type) & 31))) != 0)

#define ICMP6_FILTER_SETPASS(type, filterp) \
	((((filterp)->icmp6_filt[(type) >> 5]) &= ~(1U << ((type) & 31))))

#define ICMP6_FILTER_SETBLOCK(type, filterp) \
	((((filterp)->icmp6_filt[(type) >> 5]) |=  (1U << ((type) & 31))))

#define ICMP6_FILTER_SETPASSALL(filterp) \
	memset (filterp, 0, sizeof (struct icmp6_filter));

#define ICMP6_FILTER_SETBLOCKALL(filterp) \
	memset (filterp, 0xFF, sizeof (struct icmp6_filter));

#define ND_ROUTER_SOLICIT 133
#define ND_ROUTER_ADVERT 134
#define ND_NEIGHBOR_SOLICIT 135
#define ND_NEIGHBOR_ADVERT 136
#define ND_REDIRECT 137

struct nd_router_solicit {
	struct icmp6_hdr nd_rs_hdr;
};

#define nd_rs_type nd_rs_hdr.icmp6_type
#define nd_rs_code nd_rs_hdr.icmp6_code
#define nd_rs_cksum nd_rs_hdr.icmp6_cksum
#define nd_rs_reserved nd_rs_hdr.icmp6_data32[0]

struct nd_router_advert {
	struct icmp6_hdr nd_ra_hdr;
	uint32_t nd_ra_reachable;
	uint32_t nd_ra_retransmit;
};

struct nd_opt_hdr {
	uint8_t nd_opt_type;
	uint8_t nd_opt_len;
};

#define ND_OPT_SOURCE_LINKADDR 1
#define ND_OPT_TARGET_LINKADDR 2
#define ND_OPT_PREFIX_INFORMATION 3
#define ND_OPT_REDIRECTED_HEADER 4
#define ND_OPT_MTU 5
#define ND_OPT_RTR_ADV_INTERVAL 7
#define ND_OPT_HOME_AGENT_INFO 8

struct nd_opt_prefix_info {
	uint8_t nd_opt_pi_type;
	uint8_t nd_opt_pi_len;
	uint8_t nd_opt_pi_prefix_len;
	uint8_t nd_opt_pi_flags_reserved;
	uint32_t nd_opt_pi_valid_time;
	uint32_t nd_opt_pi_preferred_time;
	uint32_t nd_opt_pi_reserved2;
	struct in6_addr nd_opt_pi_prefix;
};

#define ND_OPT_PI_FLAG_RADDR 0x20
#define ND_OPT_PI_FLAG_AUTO 0x40
#define ND_OPT_PI_FLAG_ONLINK 0x80

#define nd_ra_type nd_ra_hdr.icmp6_type
#define nd_ra_code nd_ra_hdr.icmp6_code
#define nd_ra_cksum nd_ra_hdr.icmp6_cksum
#define nd_ra_curhoplimit nd_ra_hdr.icmp6_data8[0]
#define nd_ra_flags_reserved nd_ra_hdr.icmp6_data8[1]
#define nd_ra_router_lifetime nd_ra_hdr.icmp6_data16[1]

#define ND_RA_FLAG_HOME_AGENT 0x20
#define ND_RA_FLAG_OTHER 0x40
#define ND_RA_FLAG_MANAGED 0x80

struct nd_opt_rd_hdr {
	uint8_t nd_opt_rh_type;
	uint8_t nd_opt_rh_len;
	uint16_t nd_opt_rh_reserved1;
	uint32_t nd_opt_rh_reserved2;
};

struct nd_opt_mtu {
	uint8_t nd_opt_mtu_type;
	uint8_t nd_opt_mtu_len;
	uint16_t nd_opt_mtu_reserved;
	uint32_t nd_opt_mtu_mtu;
};

struct nd_neighbor_solicit {
	struct icmp6_hdr nd_ns_hdr;
	struct in6_addr nd_ns_target;
};

#define nd_ns_type nd_ns_hdr.icmp6_type
#define nd_ns_code nd_ns_hdr.icmp6_code
#define nd_ns_cksum nd_ns_hdr.icmp6_cksum

struct nd_neighbor_advert {
	struct icmp6_hdr nd_na_hdr;
	struct in6_addr nd_na_target;
};
#define nd_na_type nd_na_hdr.icmp6_type
#define nd_na_code nd_na_hdr.icmp6_code
#define nd_na_cksum nd_na_hdr.icmp6_cksum
#define nd_na_flags_reserved nd_na_hdr.icmp6_data32[0]

struct nd_redirect {
	struct icmp6_hdr nd_rd_hdr;
	struct in6_addr nd_rd_target;
	struct in6_addr nd_rd_dst;
};

#define nd_rd_type nd_rd_hdr.icmp6_type
#define nd_rd_code nd_rd_hdr.icmp6_code
#define nd_rd_cksum nd_rd_hdr.icmp6_cksum

#define ND_NA_FLAG_OVERRIDE 0x00000020
#define ND_NA_FLAG_SOLICITED 0x00000040
#define ND_NA_FLAG_ROUTER 0x00000080

struct nd_opt_home_agent_info {
	uint8_t nd_opt_home_agent_info_type;
	uint8_t nd_opt_home_agent_info_len;
	uint16_t nd_opt_home_agent_info_reserved;
	uint16_t nd_opt_home_agent_info_preference;
	uint16_t nd_opt_home_agent_info_lifetime;
};

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_ICMP6_H */

