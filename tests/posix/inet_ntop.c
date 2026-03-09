#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static void check(unsigned char *ip, int proto, const char *expected, char *ip_buf, size_t ip_buf_len) {
	assert(inet_ntop(proto, ip, ip_buf, ip_buf_len) == ip_buf);
	if (strcmp(ip_buf, expected)) {
		fprintf(stderr, "expected='%s' got='%s'\n", expected, ip_buf);
		assert(!strcmp(ip_buf, expected));
	}
}

int main() {
	char buf[INET_ADDRSTRLEN];
	struct in_addr addr;
	addr.s_addr = (1 << 24) | (1 << 16) | (1 << 8) | 1;
	check((uint8_t *) &addr.s_addr, AF_INET, "1.1.1.1", buf, sizeof(buf));

	char ip6_buf[INET6_ADDRSTRLEN];
	struct in6_addr addr2 = { .s6_addr = {0x20, 0x1, 0xd, 0xb8, 0, 0, 0, 0, 0, 0x1, 0, 0, 0, 0, 0, 0x1} };
	check(addr2.s6_addr, AF_INET6, "2001:db8::1:0:0:1", ip6_buf, sizeof(ip6_buf));

	struct in6_addr addr3 = { .s6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} };
	check(addr3.s6_addr, AF_INET6, "::1", ip6_buf, sizeof(ip6_buf));

	struct in6_addr addr4 = { .s6_addr = {0x20, 0x1, 0xd, 0xb8, 00, 00, 00, 0x1, 00, 0x1, 00, 0x1, 00, 0x1, 00, 0x1} };
	check(addr4.s6_addr, AF_INET6, "2001:db8:0:1:1:1:1:1", ip6_buf, sizeof(ip6_buf));

	struct in6_addr addr5 = { .s6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x01, 0x02, 0x03, 0x04}};
	check(addr5.s6_addr, AF_INET6, "::ffff:1.2.3.4", ip6_buf, sizeof(ip6_buf));

	return 0;
}
