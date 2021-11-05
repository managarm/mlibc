#ifndef _RTNETLINK_H
#define _RTNETLINK_H

#include <linux/netlink.h>

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
	unsigned short	rta_len;
	unsigned short	rta_type;
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

struct rtmsg {
	unsigned char		rtm_family;
	unsigned char		rtm_dst_len;
	unsigned char		rtm_src_len;
	unsigned char		rtm_tos;

	unsigned char		rtm_table;
	unsigned char		rtm_protocol;
	unsigned char		rtm_scope;
	unsigned char		rtm_type;

	unsigned		rtm_flags;
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

struct rtgenmsg {
	unsigned char		rtgen_family;
};

#endif // _RTNETLINK_H