#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {

struct ifaddrs *alloc_node(const char *name, size_t addrspace) {
	size_t sz = sizeof(struct ifaddrs) + IF_NAMESIZE + addrspace;
	char *block = (char *)malloc(sz);
	if (!block)
		return nullptr;
	memset(block, 0, sz);
	struct ifaddrs *ifa = (struct ifaddrs *)block;
	char *namep = block + sizeof(struct ifaddrs);
	strlcpy(namep, name, IF_NAMESIZE);
	ifa->ifa_name = namep;
	return ifa;
}

void *node_addr(struct ifaddrs *ifa) {
	return (char *)ifa + sizeof(struct ifaddrs) + IF_NAMESIZE;
}

void append(struct ifaddrs **head, struct ifaddrs **tail, struct ifaddrs *ifa) {
	if (*tail)
		(*tail)->ifa_next = ifa;
	else
		*head = ifa;
	*tail = ifa;
}

bool add_iface(int fd, struct ifreq *src, struct ifaddrs **head,
    struct ifaddrs **tail) {
	char name[IF_NAMESIZE];
	strlcpy(name, src->ifr_name, IF_NAMESIZE);

	struct ifreq req;
	short flags = 0;
	memset(&req, 0, sizeof(req));
	strlcpy(req.ifr_name, name, IF_NAMESIZE);
	if (ioctl(fd, SIOCGIFFLAGS, &req) == 0)
		flags = req.ifr_flags;

	struct ifaddrs *ll = alloc_node(name, sizeof(struct sockaddr_ll));
	if (!ll)
		return false;
	struct sockaddr_ll *sll = (struct sockaddr_ll *)node_addr(ll);
	sll->sll_family = AF_PACKET;
	sll->sll_hatype = ARPHRD_ETHER;
	sll->sll_halen = 6;

	memset(&req, 0, sizeof(req));
	strlcpy(req.ifr_name, name, IF_NAMESIZE);
	if (ioctl(fd, SIOCGIFINDEX, &req) == 0)
		sll->sll_ifindex = req.ifr_ifindex;

	memset(&req, 0, sizeof(req));
	strlcpy(req.ifr_name, name, IF_NAMESIZE);
	if (ioctl(fd, SIOCGIFHWADDR, &req) == 0)
		memcpy(sll->sll_addr, req.ifr_hwaddr.sa_data, 6);

	ll->ifa_addr = (struct sockaddr *)sll;
	ll->ifa_flags = (unsigned int)flags;
	append(head, tail, ll);

	struct sockaddr_in *cfg = (struct sockaddr_in *)(void *)&src->ifr_addr;
	if (cfg->sin_family != AF_INET || cfg->sin_addr.s_addr == 0)
		return true;

	struct ifaddrs *in = alloc_node(name, 2 * sizeof(struct sockaddr_in));
	if (!in)
		return false;
	struct sockaddr_in *addr = (struct sockaddr_in *)node_addr(in);
	struct sockaddr_in *netmask = addr + 1;

	memcpy(addr, cfg, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	in->ifa_addr = (struct sockaddr *)addr;

	memset(&req, 0, sizeof(req));
	strlcpy(req.ifr_name, name, IF_NAMESIZE);
	if (ioctl(fd, SIOCGIFNETMASK, &req) == 0) {
		memcpy(netmask, &req.ifr_netmask, sizeof(struct sockaddr_in));
		netmask->sin_family = AF_INET;
		in->ifa_netmask = (struct sockaddr *)netmask;
	}

	in->ifa_flags = (unsigned int)flags;
	append(head, tail, in);
	return true;
}

}

int getifaddrs(struct ifaddrs **ifap) {
	*ifap = nullptr;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return -1;

	struct ifconf ifc;
	memset(&ifc, 0, sizeof(ifc));
	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
		close(fd);
		return -1;
	}

	char *buf = nullptr;
	if (ifc.ifc_len > 0) {
		buf = (char *)malloc((size_t)ifc.ifc_len);
		if (!buf) {
			close(fd);
			errno = ENOMEM;
			return -1;
		}
		ifc.ifc_buf = buf;
		if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
			free(buf);
			close(fd);
			return -1;
		}
	}

	size_t count = (size_t)ifc.ifc_len / sizeof(struct ifreq);
	struct ifaddrs *head = nullptr, *tail = nullptr;

	for (size_t i = 0; i < count; i++) {
		if (!add_iface(fd, ((struct ifreq *)buf) + i, &head, &tail)) {
			freeifaddrs(head);
			free(buf);
			close(fd);
			errno = ENOMEM;
			return -1;
		}
	}

	free(buf);
	close(fd);
	*ifap = head;
	return 0;
}

void freeifaddrs(struct ifaddrs *ifa) {
	while (ifa != nullptr) {
		struct ifaddrs *next = ifa->ifa_next;
		free(ifa);
		ifa = next;
	}
}
