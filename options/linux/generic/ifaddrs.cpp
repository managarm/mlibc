#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <ifaddrs.h>
#include <errno.h>

int getifaddrs(struct ifaddrs **) {
	mlibc::infoLogger() << "mlibc: getifaddrs fails unconditionally!" << frg::endlog;
	errno = ENOSYS;
	return -1;
}

void freeifaddrs(struct ifaddrs *) {
	mlibc::infoLogger() << "mlibc: freeifaddrs is a stub!" << frg::endlog;
	return;
}
