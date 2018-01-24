
#include <arpa/inet.h>
#include <bits/ensure.h>

uint32_t htonl(uint32_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uint16_t htons(uint16_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uint32_t ntohl(uint32_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uint16_t ntohs(uint16_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// IPv4 address manipulation.
// ----------------------------------------------------------------------------
in_addr_t inet_addr(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *inet_ntoa(struct in_addr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int inet_aton(const char *, struct in_addr *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// Generic IP address manipulation.
// ----------------------------------------------------------------------------
const char *inet_ntop(int, const void *__restrict, char *__restrict,
		socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int inet_pton(int, const char *__restrict, void *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

