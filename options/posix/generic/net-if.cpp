#include <errno.h>
#include <net/if.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

void if_freenameindex(struct if_nameindex *) {
	mlibc::infoLogger() << "mlibc: if_freenameindex is a no-op" << frg::endlog;
}

char *if_indextoname(unsigned int index, char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_if_indextoname, NULL);

	if(int e = sysdep(index, name); e) {
		errno = e;
		return NULL;
	}

	return name;
}

struct if_nameindex *if_nameindex(void) {
	mlibc::infoLogger() << "mlibc: if_nameindex() is a no-op" << frg::endlog;
	errno = ENOSYS;
	return NULL;
}

unsigned int if_nametoindex(const char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_if_nametoindex, 0);
	unsigned int ret = 0;

	if(int e = sysdep(name, &ret); e) {
		errno = e;
		return 0;
	}

	return ret;
}
