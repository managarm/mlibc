#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
#include <mlibc-config.h>
#endif

#include <netdb.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if __MLIBC_LINUX_OPTION || defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)
#include <ifaddrs.h>

static bool has_ipv4_addr(void) {
	struct ifaddrs *addrs;
	if (getifaddrs(&addrs)) {
		return false;
	}

	int found = 0;
	for (struct ifaddrs *cur = addrs; cur; cur = cur->ifa_next) {
		if (cur->ifa_addr && (cur->ifa_flags & IFF_UP) && cur->ifa_addr->sa_family == AF_INET && strncmp(cur->ifa_name, "lo", IF_NAMESIZE)) {
			found = 1;
			break;
		}
	}

	freeifaddrs(addrs);
	return found;
}

static bool has_ipv6_addr(void) {
	struct ifaddrs *addrs;
	if (getifaddrs(&addrs)) {
		return false;
	}

	int found = 0;
	for (struct ifaddrs *cur = addrs; cur; cur = cur->ifa_next) {
		if (cur->ifa_addr && (cur->ifa_flags & IFF_UP) && cur->ifa_addr->sa_family == AF_INET6 && strncmp(cur->ifa_name, "lo", IF_NAMESIZE)) {
			found = 1;
			break;
		}
	}

	freeifaddrs(addrs);
	return found;
}
#endif // __MLIBC_LINUX_OPTION || defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)

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
	res = NULL;

	hints.ai_family = AF_INET6;
	hints.ai_flags = 0;
	ret = getaddrinfo("example.net", "443", &hints, &res);
	assert(ret == 0);

	assert(res[0].ai_family == AF_INET6);
	addr6 = (struct sockaddr_in6*)res[0].ai_addr;
	assert(addr6->sin6_port == htons(443));

	freeaddrinfo(res);
	res = NULL;

#if __MLIBC_LINUX_OPTION || defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)
	// Test with AF_INET
	hints.ai_family = AF_INET;
	ret = getaddrinfo("localhost", NULL, &hints, &res);
	if (has_ipv4_addr()) {
		assert(ret == 0);
		for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
			assert(p->ai_family == AF_INET);
		}
	} else {
		assert(ret != 0);
	}
	freeaddrinfo(res);
	res = NULL;

	// Test with AF_INET6
	hints.ai_family = AF_INET6;
	ret = getaddrinfo("localhost", NULL, &hints, &res);
	assert(ret == 0);
	for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
		assert(p->ai_family == AF_INET6);
	}
	freeaddrinfo(res);
	res = NULL;

	// Test AI_ADDRCONFIG
	hints = (struct addrinfo){0};
	hints.ai_flags = AI_ADDRCONFIG;
	hints.ai_family = AF_UNSPEC;

	ret = getaddrinfo("localhost", NULL, &hints, &res);

	// AI_ADDRCONFIG must not make a name unresolvable on a host whose only
	// interface is the loopback one, so the lookup has to succeed either way.
	assert(ret == 0);
	{
		int found_ipv4 = 0;
		int found_ipv6 = 0;
		for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
			if (p->ai_family == AF_INET)
				found_ipv4 = 1;
			else if (p->ai_family == AF_INET6)
				found_ipv6 = 1;
		}

		// Where exactly one family is configured, only that one is returned.
		// Where neither is, no filtering happens at all.
		if (has_ipv4_addr() != has_ipv6_addr()) {
			assert(found_ipv4 == has_ipv4_addr());
			assert(found_ipv6 == has_ipv6_addr());
		} else {
			assert(found_ipv4 || found_ipv6);
		}
	}

	freeaddrinfo(res);
	res = NULL;
#endif // __MLIBC_LINUX_OPTION || defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)

	// Test AI_NUMERICSERV.
	hints = (struct addrinfo){0};
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_family = AF_INET;

	ret = getaddrinfo(NULL, "8080", &hints, &res);
	assert(ret == 0);
	assert(res->ai_family == AF_INET);
	struct sockaddr_in *addr_in = (struct sockaddr_in *)res->ai_addr;
	assert(addr_in->sin_port == htons(8080));
	freeaddrinfo(res);
	res = NULL;

	ret = getaddrinfo(NULL, "http", &hints, &res);
	assert(ret == EAI_NONAME);
	assert(res == NULL);

	// Without a node, a client lookup resolves to the loopback address and a
	// passive one to the wildcard address. Both are stored in network byte
	// order.
	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(NULL, "8080", &hints, &res);
	assert(ret == 0);
	assert(res->ai_family == AF_INET);
	addr_in = (struct sockaddr_in *)res->ai_addr;
	assert(addr_in->sin_addr.s_addr == htonl(INADDR_LOOPBACK));
	freeaddrinfo(res);
	res = NULL;

	hints.ai_flags = AI_PASSIVE;
	ret = getaddrinfo(NULL, "8080", &hints, &res);
	assert(ret == 0);
	addr_in = (struct sockaddr_in *)res->ai_addr;
	assert(addr_in->sin_addr.s_addr == htonl(INADDR_ANY));
	freeaddrinfo(res);
	res = NULL;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(NULL, "8080", &hints, &res);
	assert(ret == 0);
	assert(res->ai_family == AF_INET6);
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)res->ai_addr;
	assert(!memcmp(&sin6->sin6_addr, &in6addr_loopback, sizeof(struct in6_addr)));
	freeaddrinfo(res);
	res = NULL;

	hints.ai_flags = AI_PASSIVE;
	ret = getaddrinfo(NULL, "8080", &hints, &res);
	assert(ret == 0);
	sin6 = (struct sockaddr_in6 *)res->ai_addr;
	assert(!memcmp(&sin6->sin6_addr, &in6addr_any, sizeof(struct in6_addr)));
	freeaddrinfo(res);
	res = NULL;

	hints = (struct addrinfo){0};
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_family = AF_INET6;

	ret = getaddrinfo(NULL, "9090", &hints, &res);
	assert(ret == 0);
	assert(res->ai_family == AF_INET6);
	struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)res->ai_addr;
	assert(addr_in6->sin6_port == htons(9090));
	freeaddrinfo(res);
	res = NULL;

	ret = getaddrinfo(NULL, "https", &hints, &res);
	assert(ret == EAI_NONAME);
	assert(res == NULL);

	// Test for NXDOMAIN
	hints = (struct addrinfo){0};
	ret = getaddrinfo("this-domain-should-not-exist.nxdomain", NULL, &hints, &res);
	assert(ret == EAI_NONAME);
	assert(res == NULL);

	// Test AI_CANONNAME.
	hints = (struct addrinfo){0};
	hints.ai_flags = AI_CANONNAME;
	ret = getaddrinfo("localhost.localdomain", NULL, &hints, &res);
	assert(ret == 0);
	assert(res->ai_canonname);
	freeaddrinfo(res);
	res = NULL;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;

	ret = getaddrinfo("127.0.0.1", "80", &hints, &res);
	assert(ret == 0);

	int count = 0;
	bool found_tcp = false;
	bool found_udp = false;

	for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
		count++;
		if (p->ai_socktype == SOCK_STREAM && p->ai_protocol == IPPROTO_TCP)
			found_tcp = true;
		if (p->ai_socktype == SOCK_DGRAM && p->ai_protocol == IPPROTO_UDP)
			found_udp = true;

		assert(p->ai_family == AF_INET);
		struct sockaddr_in *sin = (struct sockaddr_in *)p->ai_addr;
		assert(sin->sin_port == htons(80));
	}

	assert(count >= 2);
	assert(found_tcp);
	assert(found_udp);

	freeaddrinfo(res);
	res = NULL;

	return 0;
}
