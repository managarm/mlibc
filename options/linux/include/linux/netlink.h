
#ifndef _NETLINK_H
#define _NETLINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/socket.h>

#define NETLINK_ROUTE 0
#define NETLINK_USERSOCK 2
#define NETLINK_FIREWALL 3
#define NETLINK_IP6_FW 13
#define NETLINK_KOBJECT_UEVENT 15

struct sockaddr_nl {
	sa_family_t nl_family;
	unsigned short nl_pad;
	unsigned int nl_pid;
	unsigned int nl_groups;
};

struct nlmsghdr {
	unsigned int nlmsg_len;
	unsigned short nlmsg_type;
	unsigned short nlmsg_flags;
	unsigned int nlmsg_seq;
	unsigned int nlmsg_pid;
};

#define NLM_F_REQUEST	0x01

#define NLM_F_ROOT	0x100
#define NLM_F_MATCH	0x200
#define NLM_F_DUMP	(NLM_F_ROOT|NLM_F_MATCH)

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

#define NLMSG_ERROR		0x2
#define NLMSG_DONE		0x3

struct nlmsgerr {
	int		error;
	struct nlmsghdr msg;
};

#ifdef __cplusplus
}
#endif

#endif // _NETLINK_H

