#include <stdio.h>
#include <netinet/ether.h>

char *ether_ntoa_r(const struct ether_addr *addr, char *buf) {
	char *orig_ptr = buf;

	for(int i = 0; i < ETH_ALEN; i++) {
		buf += sprintf(buf, i == 0 ? "%.2X" : ":%.2X", addr->ether_addr_octet[i]);
	}

	return orig_ptr;
}
