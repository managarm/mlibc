#ifndef _RTNETLINK_H
#define _RTNETLINK_H

#include <linux/netlink.h>
#include <linux/if_addr.h>
#include <linux/if_link.h>

// Missing some defines here, but they weren't needed yet
enum {
	RTM_BASE        = 16,
#define RTM_BASE        RTM_BASE

	RTM_NEWLINK     = 16,
#define RTM_NEWLINK     RTM_NEWLINK
	RTM_DELLINK,
#define RTM_DELLINK     RTM_DELLINK
	RTM_GETLINK,
#define RTM_GETLINK     RTM_GETLINK
	RTM_SETLINK,
#define RTM_SETLINK     RTM_SETLINK

	RTM_NEWADDR     = 20,
#define RTM_NEWADDR     RTM_NEWADDR
	RTM_DELADDR,
#define RTM_DELADDR     RTM_DELADDR
	RTM_GETADDR,
#define RTM_GETADDR     RTM_GETADDR

	RTM_NEWROUTE	= 24,
#define RTM_NEWROUTE	RTM_NEWROUTE
	RTM_DELROUTE,
#define RTM_DELROUTE	RTM_DELROUTE
	RTM_GETROUTE,
#define RTM_GETROUTE	RTM_GETROUTE

	RTM_NEWNEIGH    = 28,
#define RTM_NEWNEIGH    RTM_NEWNEIGH
	RTM_DELNEIGH,
#define RTM_DELNEIGH    RTM_DELNEIGH
	RTM_GETNEIGH,
#define RTM_GETNEIGH    RTM_GETNEIGH

	RTM_NEWRULE     = 32,
#define RTM_NEWRULE     RTM_NEWRULE
	RTM_DELRULE,
#define RTM_DELRULE     RTM_DELRULE
	RTM_GETRULE,
#define RTM_GETRULE     RTM_GETRULE

	RTM_NEWQDISC    = 36,
#define RTM_NEWQDISC    RTM_NEWQDISC
	RTM_DELQDISC,
#define RTM_DELQDISC    RTM_DELQDISC
	RTM_GETQDISC,
#define RTM_GETQDISC    RTM_GETQDISC

	RTM_NEWTCLASS   = 40,
#define RTM_NEWTCLASS   RTM_NEWTCLASS
	RTM_DELTCLASS,
#define RTM_DELTCLASS   RTM_DELTCLASS
	RTM_GETTCLASS,
#define RTM_GETTCLASS   RTM_GETTCLASS

	RTM_NEWADDRLABEL = 72,
#define RTM_NEWADDRLABEL RTM_NEWADDRLABEL
	RTM_DELADDRLABEL,
#define RTM_DELADDRLABEL RTM_DELADDRLABEL
	RTM_GETADDRLABEL,
#define RTM_GETADDRLABEL RTM_GETADDRLABEL

	RTM_NEWMDB = 84,
#define RTM_NEWMDB RTM_NEWMDB
	RTM_DELMDB = 85,
#define RTM_DELMDB RTM_DELMDB
	RTM_GETMDB = 86,
#define RTM_GETMDB RTM_GETMDB

	RTM_NEWNEXTHOP = 104,
#define RTM_NEWNEXTHOP  RTM_NEWNEXTHOP
	RTM_DELNEXTHOP,
#define RTM_DELNEXTHOP  RTM_DELNEXTHOP
	RTM_GETNEXTHOP,
#define RTM_GETNEXTHOP  RTM_GETNEXTHOP

	RTM_NEWLINKPROP = 108,
#define RTM_NEWLINKPROP RTM_NEWLINKPROP
	RTM_DELLINKPROP,
#define RTM_DELLINKPROP RTM_DELLINKPROP
	RTM_GETLINKPROP,
#define RTM_GETLINKPROP RTM_GETLINKPROP
};

#define RTMGRP_IPV4_ROUTE	0x40

#define RTMGRP_IPV6_ROUTE	0x400

struct rtattr {
	unsigned short rta_len;
	unsigned short rta_type;
};

#define RTA_ALIGNTO	4U
#define RTA_ALIGN(len) ( ((len) + RTA_ALIGNTO - 1) & ~(RTA_ALIGNTO - 1) )
#define RTA_OK(rta,len) ((len) >= (int)sizeof(struct rtattr) && \
			 (rta)->rta_len >= sizeof(struct rtattr) && \
			 (rta)->rta_len <= (len))
#define RTA_NEXT(rta,attrlen)	((attrlen) -= RTA_ALIGN((rta)->rta_len), \
				 (struct rtattr*)(((char*)(rta)) + RTA_ALIGN((rta)->rta_len)))
#define RTA_LENGTH(len)	(RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_SPACE(len)	RTA_ALIGN(RTA_LENGTH(len))
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))
#define RTA_PAYLOAD(rta) ((int)((rta)->rta_len) - RTA_LENGTH(0))

struct rtmsg {
	unsigned char rtm_family;
	unsigned char rtm_dst_len;
	unsigned char rtm_src_len;
	unsigned char rtm_tos;

	unsigned char rtm_table;
	unsigned char rtm_protocol;
	unsigned char rtm_scope;
	unsigned char rtm_type;

	unsigned rtm_flags;
};

enum {
	RTN_UNSPEC,
	RTN_UNICAST,
	RTN_LOCAL,
	RTN_BROADCAST,
	RTN_ANYCAST,
	RTN_MULTICAST,
	RTN_BLACKHOLE,
	RTN_UNREACHABLE,
	RTN_PROHIBIT,
	RTN_THROW,
	RTN_NAT,
	RTN_XRESOLVE,
	__RTN_MAX
};

#define RTN_MAX (__RTN_MAX - 1)

#define RTPROT_UNSPEC 0

enum rt_scope_t {
	RT_SCOPE_UNIVERSE = 0,
	RT_SCOPE_SITE = 200,
	RT_SCOPE_LINK = 253,
	RT_SCOPE_HOST = 254,
	RT_SCOPE_NOWHERE = 255
};

enum rtattr_type_t {
	RTA_UNSPEC,
	RTA_DST,
	RTA_SRC,
	RTA_IIF,
	RTA_OIF,
	RTA_GATEWAY,
	RTA_PRIORITY,
	RTA_PREFSRC,
	RTA_METRICS,
	RTA_MULTIPATH,
	RTA_PROTOINFO,
	RTA_FLOW,
	RTA_CACHEINFO,
	RTA_SESSION,
	RTA_MP_ALGO,
	RTA_TABLE,
	RTA_MARK,
	RTA_MFC_STATS,
	RTA_VIA,
	RTA_NEWDST,
	RTA_PREF,
	RTA_ENCAP_TYPE,
	RTA_ENCAP,
	RTA_EXPIRES,
	RTA_PAD,
	RTA_UID,
	RTA_TTL_PROPAGATE,
	RTA_IP_PROTO,
	RTA_SPORT,
	RTA_DPORT,
	RTA_NH_ID,
	__RTA_MAX
};

#define RTA_MAX (__RTA_MAX - 1)

#define RTM_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct rtmsg))))

struct rtnexthop {
	unsigned short rtnh_len;
	unsigned char rtnh_flags;
	unsigned char rtnh_hops;
	int rtnh_ifindex;
};

#define RTNH_ALIGNTO    4
#define RTNH_ALIGN(len) ( ((len)+RTNH_ALIGNTO-1) & ~(RTNH_ALIGNTO-1) )
#define RTNH_OK(rtnh,len) ((rtnh)->rtnh_len >= sizeof(struct rtnexthop) && \
                           ((int)(rtnh)->rtnh_len) <= (len))
#define RTNH_NEXT(rtnh) ((struct rtnexthop*)(((char*)(rtnh)) + RTNH_ALIGN((rtnh)->rtnh_len)))
#define RTNH_LENGTH(len) (RTNH_ALIGN(sizeof(struct rtnexthop)) + (len))
#define RTNH_SPACE(len) RTNH_ALIGN(RTNH_LENGTH(len))
#define RTNH_DATA(rtnh)   ((struct rtattr*)(((char*)(rtnh)) + RTNH_LENGTH(0)))

struct rtgenmsg {
	unsigned char rtgen_family;
};

struct ifinfomsg {
	unsigned char ifi_family;
	unsigned char __ifi_pad;
	unsigned short ifi_type;
	int ifi_index;
	unsigned ifi_flags;
	unsigned ifi_change;
};

struct tcmsg {
	unsigned char tcm_family;
	unsigned char tcm__pad1;
	unsigned short tcm__pad2;
	int tcm_ifindex;
	uint32_t tcm_handle;
	uint32_t tcm_parent;
#define tcm_block_index tcm_parent
	uint32_t tcm_info;
};

#define TCM_IFINDEX_MAGIC_BLOCK (0xFFFFFFFFU)

enum {
	TCA_UNSPEC,
	TCA_KIND,
	TCA_OPTIONS,
	TCA_STATS,
	TCA_XSTATS,
	TCA_RATE,
	TCA_FCNT,
	TCA_STATS2,
	TCA_STAB,
	TCA_PAD,
	TCA_DUMP_INVISIBLE,
	TCA_CHAIN,
	TCA_HW_OFFLOAD,
	TCA_INGRESS_BLOCK,
	TCA_EGRESS_BLOCK,
	TCA_DUMP_FLAGS,
	__TCA_MAX
};

struct rta_cacheinfo {
	uint32_t rta_clntref;
	uint32_t rta_lastuse;
	int32_t rta_expires;
	uint32_t rta_error;
	uint32_t rta_used;
#define RTNETLINK_HAVE_PEERINFO 1
	uint32_t rta_id;
	uint32_t rta_ts;
	uint32_t rta_tsage;
};

enum {
	RTAX_UNSPEC,
#define RTAX_UNSPEC RTAX_UNSPEC
	RTAX_LOCK,
#define RTAX_LOCK RTAX_LOCK
	RTAX_MTU,
#define RTAX_MTU RTAX_MTU
	RTAX_WINDOW,
#define RTAX_WINDOW RTAX_WINDOW
	RTAX_RTT,
#define RTAX_RTT RTAX_RTT
	RTAX_RTTVAR,
#define RTAX_RTTVAR RTAX_RTTVAR
	RTAX_SSTHRESH,
#define RTAX_SSTHRESH RTAX_SSTHRESH
	RTAX_CWND,
#define RTAX_CWND RTAX_CWND
	RTAX_ADVMSS,
#define RTAX_ADVMSS RTAX_ADVMSS
	RTAX_REORDERING,
#define RTAX_REORDERING RTAX_REORDERING
	RTAX_HOPLIMIT,
#define RTAX_HOPLIMIT RTAX_HOPLIMIT
	RTAX_INITCWND,
#define RTAX_INITCWND RTAX_INITCWND
	RTAX_FEATURES,
#define RTAX_FEATURES RTAX_FEATURES
	RTAX_RTO_MIN,
#define RTAX_RTO_MIN RTAX_RTO_MIN
	RTAX_INITRWND,
#define RTAX_INITRWND RTAX_INITRWND
	RTAX_QUICKACK,
#define RTAX_QUICKACK RTAX_QUICKACK
	RTAX_CC_ALGO,
#define RTAX_CC_ALGO RTAX_CC_ALGO
	RTAX_FASTOPEN_NO_COOKIE,
#define RTAX_FASTOPEN_NO_COOKIE RTAX_FASTOPEN_NO_COOKIE
	__RTAX_MAX
};

#define RTAX_MAX (__RTAX_MAX - 1)

#define TCA_MAX (__TCA_MAX - 1)

#define TCA_DUMP_FLAGS_TERSE (1 << 0)

#define TCA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct tcmsg))))
#define TCA_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct tcmsg))

enum rtnetlink_groups {
	RTNLGRP_NONE,
#define RTNLGRP_NONE            RTNLGRP_NONE
	RTNLGRP_LINK,
#define RTNLGRP_LINK            RTNLGRP_LINK
	RTNLGRP_NOTIFY,
#define RTNLGRP_NOTIFY          RTNLGRP_NOTIFY
	RTNLGRP_NEIGH,
#define RTNLGRP_NEIGH           RTNLGRP_NEIGH
	RTNLGRP_TC,
#define RTNLGRP_TC              RTNLGRP_TC
	RTNLGRP_IPV4_IFADDR,
#define RTNLGRP_IPV4_IFADDR     RTNLGRP_IPV4_IFADDR
	RTNLGRP_IPV4_MROUTE,
#define RTNLGRP_IPV4_MROUTE     RTNLGRP_IPV4_MROUTE
	RTNLGRP_IPV4_ROUTE,
#define RTNLGRP_IPV4_ROUTE      RTNLGRP_IPV4_ROUTE
	RTNLGRP_IPV4_RULE,
#define RTNLGRP_IPV4_RULE       RTNLGRP_IPV4_RULE
	RTNLGRP_IPV6_IFADDR,
#define RTNLGRP_IPV6_IFADDR     RTNLGRP_IPV6_IFADDR
	RTNLGRP_IPV6_MROUTE,
#define RTNLGRP_IPV6_MROUTE     RTNLGRP_IPV6_MROUTE
	RTNLGRP_IPV6_ROUTE,
#define RTNLGRP_IPV6_ROUTE      RTNLGRP_IPV6_ROUTE
	RTNLGRP_IPV6_IFINFO,
#define RTNLGRP_IPV6_IFINFO     RTNLGRP_IPV6_IFINFO
	RTNLGRP_DECnet_IFADDR,
#define RTNLGRP_DECnet_IFADDR   RTNLGRP_DECnet_IFADDR
	RTNLGRP_NOP2,
	RTNLGRP_DECnet_ROUTE,
#define RTNLGRP_DECnet_ROUTE    RTNLGRP_DECnet_ROUTE
	RTNLGRP_DECnet_RULE,
#define RTNLGRP_DECnet_RULE     RTNLGRP_DECnet_RULE
	RTNLGRP_NOP4,
	RTNLGRP_IPV6_PREFIX,
#define RTNLGRP_IPV6_PREFIX     RTNLGRP_IPV6_PREFIX
	RTNLGRP_IPV6_RULE,
#define RTNLGRP_IPV6_RULE       RTNLGRP_IPV6_RULE
	RTNLGRP_ND_USEROPT,
#define RTNLGRP_ND_USEROPT      RTNLGRP_ND_USEROPT
	RTNLGRP_PHONET_IFADDR,
#define RTNLGRP_PHONET_IFADDR   RTNLGRP_PHONET_IFADDR
	RTNLGRP_PHONET_ROUTE,
#define RTNLGRP_PHONET_ROUTE    RTNLGRP_PHONET_ROUTE
	RTNLGRP_DCB,
#define RTNLGRP_DCB             RTNLGRP_DCB
	RTNLGRP_IPV4_NETCONF,
#define RTNLGRP_IPV4_NETCONF    RTNLGRP_IPV4_NETCONF
	RTNLGRP_IPV6_NETCONF,
#define RTNLGRP_IPV6_NETCONF    RTNLGRP_IPV6_NETCONF
	RTNLGRP_MDB,
#define RTNLGRP_MDB             RTNLGRP_MDB
	RTNLGRP_MPLS_ROUTE,
#define RTNLGRP_MPLS_ROUTE      RTNLGRP_MPLS_ROUTE
	RTNLGRP_NSID,
#define RTNLGRP_NSID            RTNLGRP_NSID
	RTNLGRP_MPLS_NETCONF,
#define RTNLGRP_MPLS_NETCONF    RTNLGRP_MPLS_NETCONF
	RTNLGRP_IPV4_MROUTE_R,
#define RTNLGRP_IPV4_MROUTE_R   RTNLGRP_IPV4_MROUTE_R
	RTNLGRP_IPV6_MROUTE_R,
#define RTNLGRP_IPV6_MROUTE_R   RTNLGRP_IPV6_MROUTE_R
	RTNLGRP_NEXTHOP,
#define RTNLGRP_NEXTHOP         RTNLGRP_NEXTHOP
	RTNLGRP_BRVLAN,
#define RTNLGRP_BRVLAN          RTNLGRP_BRVLAN
	__RTNLGRP_MAX
};
#define RTNLGRP_MAX     (__RTNLGRP_MAX - 1)

#endif // _RTNETLINK_H