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

int res_init() {
	return 0;
}

/* This is completely unused, and exists purely to satisfy broken apps. */

struct __res_state *__res_state() {
	static struct __res_state res;
	return &res;
}
