
#ifndef _NETLINK_H
#define _NETLINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc/sa_family_t.h>

#define NETLINK_KOBJECT_UEVENT 15

struct sockaddr_nl {
	sa_family_t nl_family;
	unsigned short nl_pad;
	unsigned int nl_pid;
	unsigned int nl_groups;
};

#ifdef __cplusplus
}
#endif

#endif // _NETLINK_H

