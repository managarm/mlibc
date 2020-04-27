#include <resolv.h>
#include <bits/ensure.h>

int dn_expand(const unsigned char *, const unsigned char *,
		const unsigned char *, char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int res_query(const char *, int, int, unsigned char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();	
}
