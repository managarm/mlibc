#include <mlibc/all-sysdeps.hpp>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

namespace mlibc {

int sys_if_indextoname(unsigned int index, char *name) {
    int fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC);

    if(fd < 0)
        return 0;

    struct ifreq ifr;
    ifr.ifr_ifindex = index;

    int res = 0;
	int ret = sys_ioctl(fd, SIOCGIFNAME, &ifr, &res);
    close(fd);

    if(ret < 0) {
        if(ret == ENODEV)
            return ENXIO;
		return ret;
    }

    strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
    int fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC);

    if(fd < 0)
        return -1;

    struct ifreq ifr;
    strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

    int r = ioctl(fd, SIOCGIFINDEX, &ifr);
    close(fd);

    *ret = (r < 0) ? r : ifr.ifr_ifindex;

	return 0;
}

} //namespace mlibc
