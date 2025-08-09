#include <netdb.h>
#include <assert.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int main() {
	struct addrinfo *res = NULL;
	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(NULL, "443", &hints, &res);
	assert(ret == 0);

	struct sockaddr_in *addr = (struct sockaddr_in*)(res[0].ai_addr);
	assert(addr->sin_port == htons(443));
	assert(res[0].ai_socktype == SOCK_STREAM);
	assert(res[0].ai_protocol == IPPROTO_TCP);

	freeaddrinfo(res);
	res = NULL;

	/* check we can resolve any domain */
	ret = getaddrinfo("example.net", NULL, &hints, &res);
	assert(ret == 0);

	freeaddrinfo(res);
	res = NULL;

	hints.ai_flags = AI_NUMERICHOST;
	ret = getaddrinfo("10.10.10.10", NULL, &hints, &res);
	assert(ret == 0);

	addr = (struct sockaddr_in*)res[0].ai_addr;
	assert((addr->sin_addr.s_addr & 0xFF) == 10);
	assert(((addr->sin_addr.s_addr >> 8) & 0xFF) == 10);
	assert(((addr->sin_addr.s_addr >> 16) & 0xFF) == 10);
	assert(((addr->sin_addr.s_addr >> 24) & 0xFF) == 10);

	freeaddrinfo(res);
	res = NULL;

	ret = getaddrinfo("example.net", NULL, &hints, &res);
	assert(ret == EAI_NONAME);

	freeaddrinfo(res);
	res = NULL;

	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_NUMERICHOST;
	ret = getaddrinfo("::1", "1234", &hints, &res);
	assert(ret == 0);

	assert(res[0].ai_family == AF_INET6);
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6*) res[0].ai_addr;
	assert(addr6->sin6_port == htons(1234));
	assert(addr6->sin6_addr.s6_addr[15] == 1);
	for (int i = 0; i < 15; i++) {
		assert(addr6->sin6_addr.s6_addr[i] == 0);
	}

	freeaddrinfo(res);

	return 0;
}
