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

/* This is completely unused, and exists purely to satisfy broken apps. */

struct __res_state *__res_state() {
	static struct __res_state res;
	return &res;
}
