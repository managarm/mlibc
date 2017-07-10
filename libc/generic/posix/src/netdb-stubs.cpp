
#include <netdb.h>
#include <mlibc/ensure.h>

void endhostent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endnetent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endprotoent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endservent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void freeaddrinfo(struct addrinfo *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

const char *gai_strerror(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getaddrinfo(const char *__restrict, const char *__restrict,
		const struct addrinfo *__restrict, struct addrinfo **__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct hostent *gethostent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getnameinfo(const struct sockaddr *__restrict, socklen_t,
		char *__restrict, socklen_t, char *__restrict, socklen_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct netent *getnetbyaddr(uint32_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct netent *getnetbyname(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct netent *getnetent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct protoent *getprotobyname(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct protoent *getprotobynumber(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct protoent *getprotoent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct servent *getservbyname(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct servent *getservbyport(int, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct servent *getservent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void sethostent(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void setnetent(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void setprotoent(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

