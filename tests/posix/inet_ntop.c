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
	return 0;
}
