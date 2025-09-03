#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/ioctl.h>
#include <unistd.h>

#include "generic-helpers/netlink.hpp"

namespace mlibc {

int sys_if_indextoname(unsigned int index, char *name) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int res = 0;
	int ret = sys_ioctl(fd, SIOCGIFNAME, &ifr, &res);
	close(fd);

	if (ret) {
		if (ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	int res = 0;
	r = sys_ioctl(fd, SIOCGIFINDEX, &ifr, &res);
	close(fd);

	if (r)
		return r;

	*ret = ifr.ifr_ifindex;

	return 0;
}

int sys_getifaddrs(struct ifaddrs **out) {
	NetlinkHelper nl;
	*out = nullptr;

	bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&getifaddrs_callback, out);
	__ensure(link_ret);
	bool addr_ret = nl.send_request(RTM_GETADDR) && nl.recv(&getifaddrs_callback, out);
	__ensure(addr_ret);

	return 0;
}

#if !defined(MLIBC_BUILDING_RTLD)
int sys_inet_configured(bool *ipv4, bool *ipv6) {
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
			    auto interfaceNameResult = sys_if_indextoname(ifaddr->ifa_index, name);

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
