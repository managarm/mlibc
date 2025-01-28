#include <arpa/inet.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main() {
	struct in_addr addr;
	addr.s_addr = (1 << 24) |
		(1 << 16) | (1 << 8) | 1;
	char buf[INET_ADDRSTRLEN];
	assert(inet_ntop(AF_INET, &addr, buf, INET_ADDRSTRLEN) != NULL);
	assert(strncmp("1.1.1.1", buf, INET_ADDRSTRLEN) == 0);

	struct in6_addr addr2 = { .s6_addr = {0x20, 0x1, 0xd, 0xb8, 0, 0, 0, 0, 0, 0x1, 0, 0, 0, 0, 0, 0x1} };
	char buf2[INET6_ADDRSTRLEN];
	assert(inet_ntop(AF_INET6, &addr2, buf2, INET6_ADDRSTRLEN) != NULL);
	assert(strncmp("2001:db8::1:0:0:1", buf2, INET6_ADDRSTRLEN) == 0);

	struct in6_addr addr3 = { .s6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} };
	char buf3[INET6_ADDRSTRLEN];
	assert(inet_ntop(AF_INET6, &addr3, buf3, INET6_ADDRSTRLEN) != NULL);
	assert(strncmp("::1", buf3, INET6_ADDRSTRLEN) == 0);

	struct in6_addr addr4 = { .s6_addr = {0x20, 0x1, 0xd, 0xb8, 00, 00, 00, 0x1, 00, 0x1, 00, 0x1, 00, 0x1, 00, 0x1} };
	char buf4[INET6_ADDRSTRLEN];
	assert(inet_ntop(AF_INET6, &addr4, buf4, INET6_ADDRSTRLEN) != NULL);
	assert(strncmp("2001:db8:0:1:1:1:1:1", buf4, INET6_ADDRSTRLEN) == 0);

	return 0;
}
