
#include <dirent.h>
#include <net/if.h>
#include <bits/ensure.h>

void if_freenameindex(struct if_nameindex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *if_indextoname(unsigned int, char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct if_nameindex *if_nameindex(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

unsigned int if_nametoindex(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
