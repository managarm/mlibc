#include <errno.h>
#include <net/if.h>

#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>

void if_freenameindex(struct if_nameindex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
