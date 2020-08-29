#include <arpa/inet.h>
#include <assert.h>

int main() {
	struct in_addr addr;
	assert(inet_pton(AF_INET, "1.1.1.1", &addr));
	assert((addr.s_addr & 0xFF) == 1);
	assert(((addr.s_addr >> 8) & 0xFF) == 1);
	assert(((addr.s_addr >> 16) & 0xFF) == 1);
	assert(((addr.s_addr >> 24) & 0xFF) == 1);

	assert(!inet_pton(AF_INET, "256.999.1234.555", &addr));
	assert(!inet_pton(AF_INET, "a.b.c.d", &addr));
	return 0;
}

