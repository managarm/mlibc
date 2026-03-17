#include <errno.h>
#include <net/if.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

void if_freenameindex(struct if_nameindex *) {
	mlibc::infoLogger() << "mlibc: if_freenameindex is a no-op" << frg::endlog;
}

char *if_indextoname(unsigned int index, char *name) {
	if(int e = mlibc::sysdep_or_enosys<IfIndextoname>(index, name); e) {
		errno = e;
		return nullptr;
	}

	return name;
}

struct if_nameindex *if_nameindex(void) {
	mlibc::infoLogger() << "mlibc: if_nameindex() is a no-op" << frg::endlog;
	errno = ENOSYS;
	return nullptr;
}

unsigned int if_nametoindex(const char *name) {
	unsigned int ret = 0;

	if(int e = mlibc::sysdep_or_enosys<IfNametoindex>(name, &ret); e) {
		errno = e;
		return 0;
	}

	return ret;
}
