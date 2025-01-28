
#ifndef  _IFADDRS_H
#define  _IFADDRS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include <sys/socket.h>

/* Struct definitions taken from musl */
struct ifaddrs {
	struct ifaddrs *ifa_next;
	char *ifa_name;
	unsigned ifa_flags;
	struct sockaddr *ifa_addr;
	struct sockaddr *ifa_netmask;
	/* the man page (and glibc) place `ifa_broadaddr` and `ifa_dstaddr` in a union */
	/* TODO: decide whether we should do it, too */
	struct sockaddr *ifa_broadaddr;
	struct sockaddr *ifa_dstaddr;
	void *ifa_data;
};

#ifndef __MLIBC_ABI_ONLY

int getifaddrs(struct ifaddrs **__ifap);
void freeifaddrs(struct ifaddrs *__ifa);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _IFADDRS_H */
