
#ifndef _NETLINK_H
#define _NETLINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/socket.h>
#include <stdint.h>

#define NETLINK_ROUTE 0
#define NETLINK_USERSOCK 2
#define NETLINK_FIREWALL 3
#define NETLINK_NETFILTER 12
#define NETLINK_IP6_FW 13
#define NETLINK_KOBJECT_UEVENT 15
#define NETLINK_GENERIC 16

struct sockaddr_nl {
	sa_family_t nl_family;
	unsigned short nl_pad;
	uint32_t nl_pid;
	uint32_t nl_groups;
};

struct nlmsghdr {
	uint32_t nlmsg_len;
	uint16_t nlmsg_type;
	uint16_t nlmsg_flags;
	uint32_t nlmsg_seq;
	uint32_t nlmsg_pid;
};

struct nlattr {
	uint16_t nla_len;
	uint16_t nla_type;
};

#define NLM_F_REQUEST	0x01
#define NLM_F_MULTI		0x02
#define NLM_F_ACK		0x04

#define NLM_F_ROOT		0x100
#define NLM_F_MATCH		0x200
#define NLM_F_DUMP		(NLM_F_ROOT|NLM_F_MATCH)

#define NLM_F_REPLACE	0x100
#define NLM_F_EXCL		0x200
#define NLM_F_CREATE	0x400
#define NLM_F_APPEND	0x800

#define NLM_F_CAPPED	0x100
#define NLM_F_ACK_TLVS	0x200

#define NLMSG_ALIGNTO		4U
#define NLMSG_ALIGN(len) 	(((len) + NLMSG_ALIGNTO - 1) & ~(NLMSG_ALIGNTO - 1))
#define NLMSG_HDRLEN	 	((int)NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) 	((len) + NLMSG_HDRLEN)
#define NLMSG_SPACE(len) 	NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  	((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)	((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
				  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP		0x1
#define NLMSG_ERROR		0x2
#define NLMSG_DONE		0x3
#define NLMSG_OVERRUN	0x4

#define NLMSG_MIN_TYPE	0x10

struct nlmsgerr {
	int		error;
	struct nlmsghdr msg;
};

enum nlmsgerr_attrs {
	NLMSGERR_ATTR_UNUSED,
	NLMSGERR_ATTR_MSG,
	NLMSGERR_ATTR_OFFS,
	NLMSGERR_ATTR_COOKIE,
	NLMSGERR_ATTR_POLICY,

	__NLMSGERR_ATTR_MAX,
	NLMSGERR_ATTR_MAX = __NLMSGERR_ATTR_MAX - 1
};

#define NETLINK_ADD_MEMBERSHIP			1
#define NETLINK_DROP_MEMBERSHIP			2
#define NETLINK_PKTINFO					3
#define NETLINK_LIST_MEMBERSHIPS		9
#define NETLINK_EXT_ACK                 11
#define NETLINK_GET_STRICT_CHK          12

struct nl_pktinfo {
	uint32_t group;
};

#define NLA_F_NESTED            (1 << 15)
#define NLA_F_NET_BYTEORDER     (1 << 14)
#define NLA_TYPE_MASK           ~(NLA_F_NESTED | NLA_F_NET_BYTEORDER)

#ifdef __cplusplus
}
#endif

#endif // _NETLINK_H

