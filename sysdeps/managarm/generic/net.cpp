#include <mlibc/all-sysdeps.hpp>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

namespace mlibc {

int sys_if_indextoname(unsigned int index, char *name) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int res = 0;
	int ret = sys_ioctl(fd, SIOCGIFNAME, &ifr, &res);
	close(fd);

	if(ret) {
		if(ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	int res = 0;
	r = sys_ioctl(fd, SIOCGIFINDEX, &ifr, &res);
	close(fd);

	if(r)
		return r;

	*ret = ifr.ifr_ifindex;

	return 0;
}

} //namespace mlibc
