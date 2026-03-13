#include <errno.h>
#include <net/if.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

void if_freenameindex(struct if_nameindex *p) {
	if (!p)
		return;

	for (auto c = p; c->if_index || c->if_name; c++) {
		if (c->if_name)
			getAllocator().free(c->if_name);
	}

	getAllocator().free(p);
}

char *if_indextoname(unsigned int index, char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_if_indextoname, NULL);

	if(int e = sysdep(index, name); e) {
		errno = e;
		return nullptr;
	}

	return name;
}

struct if_nameindex *if_nameindex(void) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_if_nameindex, NULL);

	struct if_nameindex *out = nullptr;
	if(int e = sysdep(&out); e) {
		errno = e;
		return nullptr;
	}

	return out;
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
