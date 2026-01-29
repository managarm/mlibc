#include <resolv.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

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
	mlibc::infoLogger() << "mlibc: res_init is a stub!" << frg::endlog;
	return 0;
}

int res_ninit(res_state) {
	mlibc::infoLogger() << "mlibc: res_ninit is a stub!" << frg::endlog;
	return 0;
}

void res_nclose(res_state) {
	mlibc::infoLogger() << "mlibc: res_nclose is a stub!" << frg::endlog;
	return;
}

int dn_comp(const char *, unsigned char *, int, unsigned char **, unsigned char **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

/* Taken from musl */
int dn_skipname(const unsigned char *s, const unsigned char *end) {
	const unsigned char *p = s;
	while (p < end)
		if (!*p)
			return p - s + 1;
		else if (*p >= 192)
			if (p + 1 < end)
				return p - s + 2;
			else
				break;
		else if (end - p < *p + 1)
			break;
		else
			p += *p + 1;
	return -1;
}

/* This is completely unused, and exists purely to satisfy broken apps. */

struct __res_state *__res_state() {
	static struct __res_state res;
	return &res;
}
