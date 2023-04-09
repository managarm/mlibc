
#ifndef  _IFADDRS_H
#define  _IFADDRS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include <sys/socket.h>

// Struct definitions taken from musl
struct ifaddrs {
	struct ifaddrs *ifa_next;
	char *ifa_name;
	unsigned ifa_flags;
	struct sockaddr *ifa_addr;
	struct sockaddr *ifa_netmask;
	struct sockaddr *ifa_broadaddr;
	struct sockaddr *ifa_dstaddr;
	void *ifa_data;
};

#ifndef __MLIBC_ABI_ONLY

int getifaddrs(struct ifaddrs **);
void freeifaddrs(struct ifaddrs *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _IFADDRS_H
