#include <errno.h>
#include <net/if.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

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
	if(int e = mlibc::sysdep_or_enosys<IfIndextoname>(index, name); e) {
		errno = e;
		return nullptr;
	}

	return name;
}

struct if_nameindex *if_nameindex(void) {
	struct if_nameindex *out = nullptr;
	if(int e = mlibc::sysdep_or_enosys<IfNameindex>(&out); e) {
		errno = e;
		return nullptr;
	}

	return out;
}

unsigned int if_nametoindex(const char *name) {
	unsigned int ret = 0;

	if(int e = mlibc::sysdep_or_enosys<IfNametoindex>(name, &ret); e) {
		errno = e;
		return 0;
	}

	return ret;
}
