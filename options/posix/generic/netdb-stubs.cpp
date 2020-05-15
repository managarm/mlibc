#include <netdb.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/lookup.hpp>
#include <sys/socket.h>
#include <stdlib.h>
#include <stddef.h>

#undef h_errno
__thread int __mlibc_h_errno;

// This function is from musl
int *__h_errno_location(void) {
	return &__mlibc_h_errno;
}

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

void freeaddrinfo(struct addrinfo *ptr) {
	auto buf = (struct mlibc::ai_buf*) ptr - offsetof(struct mlibc::ai_buf, ai);
	free(ptr->ai_canonname);
	free(buf);
}

const char *gai_strerror(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getaddrinfo(const char *__restrict node, const char *__restrict service,
		const struct addrinfo *__restrict hints, struct addrinfo **__restrict res) {
		frg::endlog;
	if (!node && !service)
		return EAI_NONAME;

	if (service) {
		__ensure(!"Not implemented");
		__builtin_unreachable();
	}

	int socktype = 0, protocol = 0;
	if (hints) {
		socktype = hints->ai_socktype;
		protocol = hints->ai_protocol;
	}

	struct mlibc::dns_addr_buf buf[NAME_MAX];
	int ret = mlibc::lookup_name_dns(buf, node);
	if (ret < 0)
		return ret;

	auto out = (struct mlibc::ai_buf *) calloc(ret, sizeof(struct addrinfo));

	auto canon_len = strlen(node);
	auto canon = (char *) malloc(canon_len);
	strncpy(canon, node, canon_len);

	for (int i = 0; i < ret; i++) {
		out[i].ai.ai_family = buf[i].family;
		out[i].ai.ai_socktype = socktype;
		out[i].ai.ai_protocol = protocol;
		out[i].ai.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
		out[i].ai.ai_addr = (struct sockaddr *) &out[i].sa;
		out[i].ai.ai_canonname = canon;
		out[i].ai.ai_next = NULL;
		switch (buf[i].family) {
			case AF_INET:
				out[i].ai.ai_addrlen = sizeof(struct sockaddr_in);
				out[i].sa.sin.sin_family = AF_INET;
				memcpy(&out[i].sa.sin.sin_addr, buf[i].addr, 4);
				break;
			case AF_INET6:
				out[i].ai.ai_addrlen = sizeof(struct sockaddr_in6);
				out[i].sa.sin6.sin6_family = AF_INET6;
				memcpy(&out[i].sa.sin6.sin6_addr, buf[i].addr, 16);
				break;
		}
	}

	*res = &out[0].ai;
	return 0;
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

struct hostent *gethostbyname(const char *) {
	__ensure(!"gethostbyname() not implemented");
	__builtin_unreachable();
}

struct hostent *gethostbyaddr(const void *, socklen_t, int) {
	__ensure(!"gethostbyaddr() not implemented");
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

void setservent(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

