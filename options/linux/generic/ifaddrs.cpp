#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <ifaddrs.h>
#include <errno.h>

int getifaddrs(struct ifaddrs **ifap) {
	if(int e = mlibc::sysdep_or_enosys<Getifaddrs>(ifap); e) {
		errno = e;
		return -1;
	}

	return 0;
}

void freeifaddrs(struct ifaddrs *ifa) {
	if constexpr (mlibc::IsImplemented<Freeifaddrs>)
		mlibc::sysdep_or_panic<Freeifaddrs>(ifa);
}
