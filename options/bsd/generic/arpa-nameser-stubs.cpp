#include <errno.h>
#include <arpa/nameser.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

// The ns_get* and ns_put* functions are taken from musl.
unsigned ns_get16(const unsigned char *cp) {
	return cp[0] << 8 | cp[1];
}

unsigned long ns_get32(const unsigned char *cp) {
	return (unsigned)cp[0] << 24 | cp[1] << 16 | cp[2] << 8 | cp[3];
}

void ns_put16(unsigned s, unsigned char *cp) {
	*cp++ = s >> 8;
	*cp++ = s;
}

void ns_put32(unsigned long l, unsigned char *cp) {
	*cp++ = l >> 24;
	*cp++ = l >> 16;
	*cp++ = l >> 8;
	*cp++ = l;
}

int ns_initparse(const unsigned char *, int, ns_msg *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int ns_parserr(ns_msg *, ns_sect, int, ns_rr *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int ns_name_uncompress(const unsigned char *, const unsigned char *,
				    const unsigned char *, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
