#include <mlibc/allocator.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <ifaddrs.h>
#include <errno.h>

int getifaddrs(struct ifaddrs **ifap) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getifaddrs, -1);
	if(int e = sysdep(ifap); e) {
		errno = e;
		return -1;
	}

	return 0;
}

void freeifaddrs(struct ifaddrs *ifa) {
	while (ifa != nullptr) {
		ifaddrs *current = ifa;
		ifa = ifa->ifa_next;
		getAllocator().free(current);
	}
}
