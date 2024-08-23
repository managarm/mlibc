#include <stdio.h>
#include <bits/ensure.h>
#include <netinet/ether.h>

char *ether_ntoa(const struct ether_addr *addr) {
	static char x[18];
	return ether_ntoa_r (addr, x);
}

char *ether_ntoa_r(const struct ether_addr *addr, char *buf) {
	char *orig_ptr = buf;

	for(int i = 0; i < ETH_ALEN; i++) {
		buf += sprintf(buf, i == 0 ? "%.2X" : ":%.2X", addr->ether_addr_octet[i]);
	}

	return orig_ptr;
}

struct ether_addr *ether_aton(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
