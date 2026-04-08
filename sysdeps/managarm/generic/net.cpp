#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

#include "generic-helpers/netlink.hpp"

namespace mlibc {

int Sysdeps<IfIndextoname>::operator()(unsigned int index, char *name) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int res = 0;
	int ret = sysdep<Ioctl>(fd, SIOCGIFNAME, &ifr, &res);
	close(fd);

	if (ret) {
		if (ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int Sysdeps<IfNametoindex>::operator()(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	int res = 0;
	r = sysdep<Ioctl>(fd, SIOCGIFINDEX, &ifr, &res);
	close(fd);

	if (r)
		return r;

	*ret = ifr.ifr_ifindex;

	return 0;
}

int Sysdeps<IfNameindex>::operator()(struct if_nameindex **out) {
	NetlinkHelper nl;
	frg::vector<struct if_nameindex, MemoryAllocator> list{getAllocator()};

	bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&if_nameindex_callback, &list);
	__ensure(link_ret);

	list.emplace_back(0, nullptr);
	*out = list.data();
	list.detach();

	return 0;
}

void Sysdeps<IfFreeNameindex>::operator()(struct if_nameindex *ifn) {
	if (!ifn)
		return;

	for (auto c = ifn; c->if_index || c->if_name; c++) {
		if (c->if_name)
			getAllocator().free(c->if_name);
	}

	getAllocator().free(ifn);
}

int Sysdeps<Getifaddrs>::operator()(struct ifaddrs **out) {
	NetlinkHelper nl;
	*out = nullptr;

	bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&getifaddrs_callback, out);
	__ensure(link_ret);
	bool addr_ret = nl.send_request(RTM_GETADDR) && nl.recv(&getifaddrs_callback, out);
	__ensure(addr_ret);

	return 0;
}

void Sysdeps<Freeifaddrs>::operator()(struct ifaddrs *ifa) {
	while (ifa != nullptr) {
		ifaddrs *current = ifa;
		ifa = ifa->ifa_next;
		frg::destruct(getAllocator(), reinterpret_cast<IfaddrHelper *>(current));
	}
}

#if !defined(MLIBC_BUILDING_RTLD)
int Sysdeps<InetConfigured>::operator()(bool *ipv4, bool *ipv6) {
	struct context {
		bool *ipv4;
		bool *ipv6;
	} context = {.ipv4 = ipv4, .ipv6 = ipv6};

	NetlinkHelper nl;
	if (!nl.send_request(RTM_GETADDR)) {
		*ipv4 = false;
		*ipv6 = false;
		return 0;
	}

	auto ret = nl.recv(
	    [](void *data, const nlmsghdr *hdr) {
		    if (hdr->nlmsg_type == RTM_NEWADDR || hdr->nlmsg_len >= sizeof(struct ifaddrmsg)) {
			    const struct ifaddrmsg *ifaddr =
			        reinterpret_cast<const struct ifaddrmsg *>(NLMSG_DATA(hdr));
			    struct context *ctx = reinterpret_cast<struct context *>(data);

			    char name[IF_NAMESIZE];
			    auto interfaceNameResult = sysdep<IfIndextoname>(ifaddr->ifa_index, name);

			    if (interfaceNameResult || !strncmp(name, "lo", IF_NAMESIZE))
				    return;

			    if (ifaddr->ifa_family == AF_INET)
				    *ctx->ipv4 = true;
			    else if (ifaddr->ifa_family == AF_INET6)
				    *ctx->ipv6 = true;
		    }
	    },
	    &context
	);

	if (!ret) {
		*ipv4 = false;
		*ipv6 = false;
		return 0;
	}

	return 0;
}
#endif // !defined(MLIBC_BUILDING_RTLD)

} // namespace mlibc
