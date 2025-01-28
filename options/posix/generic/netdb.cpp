#include <netdb.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/lookup.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/services.hpp>
#include <frg/vector.hpp>
#include <frg/array.hpp>
#include <frg/span.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>

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
	if (ptr) {
		auto buf = (struct mlibc::ai_buf*) ptr - offsetof(struct mlibc::ai_buf, ai);
		// this string was allocated by a frg::string
		getAllocator().free(ptr->ai_canonname);
		free(buf);
	}
}

const char *gai_strerror(int code) {
	static thread_local char buffer[128];
	snprintf(buffer, sizeof(buffer), "Unknown error (%d)", code);
	return buffer;
}

int getaddrinfo(const char *__restrict node, const char *__restrict service,
		const struct addrinfo *__restrict hints, struct addrinfo **__restrict res) {
	if (!node && !service)
		return EAI_NONAME;

	int socktype = 0, protocol = 0, family = AF_UNSPEC, flags = AI_V4MAPPED | AI_ADDRCONFIG;
	if (hints) {
		socktype = hints->ai_socktype;
		protocol = hints->ai_protocol;
		family = hints->ai_family;
		flags = hints->ai_flags;

		int mask = AI_V4MAPPED | AI_ADDRCONFIG | AI_NUMERICHOST | AI_PASSIVE |
			AI_CANONNAME | AI_ALL | AI_NUMERICSERV;
		if ((flags & mask) != flags)
			return EAI_BADFLAGS;

		if (family != AF_INET && family != AF_INET6 && family != AF_UNSPEC)
			return EAI_FAMILY;
	}

	mlibc::service_result serv_buf{getAllocator()};
	int serv_count = mlibc::lookup_serv_by_name(serv_buf, service, protocol, socktype, flags);
	if (serv_count < 0)
		return -serv_count;

	struct mlibc::lookup_result addr_buf;
	int addr_count = 1;
	frg::string<MemoryAllocator> canon{getAllocator()};
	if (node) {
		if ((addr_count = mlibc::lookup_name_ip(addr_buf, node, family)) <= 0) {
			if (flags & AI_NUMERICHOST)
			       addr_count = -EAI_NONAME;
			else if ((addr_count = mlibc::lookup_name_hosts(addr_buf, node, canon)) <= 0)
				addr_count = mlibc::lookup_name_dns(addr_buf, node, canon);
			else
				addr_count = 1;
		}

		if (addr_count < 0)
			return -addr_count;
		if (!addr_count)
			return EAI_NONAME;
	} else {
		/* There is no node specified */
		if (flags & AI_NUMERICHOST)
			return EAI_NONAME;
		addr_count = lookup_name_null(addr_buf, flags, family);
	}

	auto out = (struct mlibc::ai_buf *) calloc(serv_count * addr_count,
			sizeof(struct mlibc::ai_buf));

	if (node && !canon.size())
		canon = frg::string<MemoryAllocator>{node, getAllocator()};

	for (int i = 0, k = 0; i < addr_count; i++) {
		for (int j = 0; j < serv_count; j++, k++) {
			out[i].ai.ai_family = addr_buf.buf[i].family;
			out[i].ai.ai_socktype = serv_buf[j].socktype;
			out[i].ai.ai_protocol = serv_buf[j].protocol;
			out[i].ai.ai_flags = flags;
			out[i].ai.ai_addr = (struct sockaddr *) &out[i].sa;
			if (canon.size())
				out[i].ai.ai_canonname = canon.data();
			else
				out[i].ai.ai_canonname = NULL;
			out[i].ai.ai_next = NULL;
			switch (addr_buf.buf[i].family) {
				case AF_INET:
					out[i].ai.ai_addrlen = sizeof(struct sockaddr_in);
					out[i].sa.sin.sin_port = htons(serv_buf[j].port);
					out[i].sa.sin.sin_family = AF_INET;
					memcpy(&out[i].sa.sin.sin_addr, addr_buf.buf[i].addr, 4);
					break;
				case AF_INET6:
					out[i].ai.ai_addrlen = sizeof(struct sockaddr_in6);
					out[i].sa.sin6.sin6_family = htons(serv_buf[j].port);
					out[i].sa.sin6.sin6_family = AF_INET6;
					memcpy(&out[i].sa.sin6.sin6_addr, addr_buf.buf[i].addr, 16);
					break;
			}
		}
	}
	if (canon.size())
		canon.detach();

	*res = &out[0].ai;
	return 0;
}

struct hostent *gethostent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getnameinfo(const struct sockaddr *__restrict addr, socklen_t addr_len,
		char *__restrict host, socklen_t host_len, char *__restrict serv,
		socklen_t serv_len, int flags) {
	frg::array<uint8_t, 16> addr_array;
	int family = addr->sa_family;

	switch(family) {
		case AF_INET: {
			if (addr_len < sizeof(struct sockaddr_in))
				return EAI_FAMILY;
			auto sockaddr = reinterpret_cast<const struct sockaddr_in*>(addr);
			memcpy(addr_array.data(), reinterpret_cast<const char*>(&sockaddr->sin_addr), 4);
			break;
		}
		case AF_INET6: {
			mlibc::infoLogger() << "getnameinfo(): ipv6 is not fully supported in this function" << frg::endlog;
			if (addr_len < sizeof(struct sockaddr_in6))
				return EAI_FAMILY;
			auto sockaddr = reinterpret_cast<const struct sockaddr_in6*>(addr);
			memcpy(addr_array.data(), reinterpret_cast<const char*>(&sockaddr->sin6_addr), 16);
			break;
		}
		default:
			return EAI_FAMILY;
	}

	if (host && host_len) {
		frg::span<char> host_span{host, host_len};
		int res = 0;
		if (!(flags & NI_NUMERICHOST))
			res = mlibc::lookup_addr_hosts(host_span, addr_array, family);
		if (!(flags & NI_NUMERICHOST) && !res)
			res = mlibc::lookup_addr_dns(host_span, addr_array, family);

		if (!res) {
			if (flags & NI_NAMEREQD)
				return EAI_NONAME;
			if(!inet_ntop(family, addr_array.data(), host, host_len)) {
				switch(errno) {
					case EAFNOSUPPORT:
						return EAI_FAMILY;
					case ENOSPC:
						return EAI_OVERFLOW;
					default:
						return EAI_FAIL;
				}
			}
		}

		if (res < 0)
			return -res;
	}

	if (serv && serv_len) {
		__ensure("getnameinfo(): not implemented service resolution yet!");
		__builtin_unreachable();
	}

	return 0;
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

struct hostent *gethostbyname(const char *name) {
	if (!name) {
		h_errno = HOST_NOT_FOUND;
		return NULL;
	}

	struct mlibc::lookup_result buf;
	frg::string<MemoryAllocator> canon{getAllocator()};
	int ret = 0;
	if ((ret = mlibc::lookup_name_hosts(buf, name, canon)) <= 0)
		ret = mlibc::lookup_name_dns(buf, name, canon);
	if (ret <= 0) {
		h_errno = HOST_NOT_FOUND;
		return NULL;
	}

	static struct hostent h;
	if (h.h_name) {
		getAllocator().free(h.h_name);
		for (int i = 0; h.h_aliases[i] != NULL; i++)
			getAllocator().free(h.h_aliases[i]);
		free(h.h_aliases);

		if (h.h_addr_list) {
			for (int i = 0; h.h_addr_list[i] != NULL; i++)
				free(h.h_addr_list[i]);
			free(h.h_addr_list);
		}
	}
	h = {};

	if (!canon.size())
		canon = frg::string<MemoryAllocator>{name, getAllocator()};

	h.h_name = canon.data();

	h.h_aliases = reinterpret_cast<char**>(malloc((buf.aliases.size() + 1)
				* sizeof(char*)));
	int alias_pos = 0;
	for (auto &buf_name : buf.aliases) {
		h.h_aliases[alias_pos] = buf_name.data();
		buf_name.detach();
		alias_pos++;
	}
	h.h_aliases[alias_pos] = NULL;
	canon.detach();

	// just pick the first family as the one for all addresses...??
	h.h_addrtype = buf.buf[0].family;
	if (h.h_addrtype != AF_INET && h.h_addrtype != AF_INET6) {
		// this is not allowed per spec
		h_errno = NO_DATA;
		return NULL;
	}

	// can only be AF_INET or AF_INET6
	h.h_length = h.h_addrtype == AF_INET ? 4 : 16;
	h.h_addr_list = reinterpret_cast<char**>(malloc((ret + 1) * sizeof(char*)));
	int addr_pos = 0;
	for (int i = 0; i < ret; i++) {
		if (buf.buf[i].family != h.h_addrtype)
			continue;
		h.h_addr_list[addr_pos] = reinterpret_cast<char*>(malloc(h.h_length));
		memcpy(h.h_addr_list[addr_pos], buf.buf[i].addr, h.h_length);
		addr_pos++;
	}
	h.h_addr_list[addr_pos] = NULL;

	return &h;
}

struct hostent *gethostbyname2(const char *, int) {
	__ensure(!"gethostbyname2() not implemented");
	__builtin_unreachable();
}

struct hostent *gethostbyaddr(const void *, socklen_t, int) {
	__ensure(!"gethostbyaddr() not implemented");
	__builtin_unreachable();
}

int gethostbyaddr_r(const void *__restrict, socklen_t, int, struct hostent *__restrict,
					char *__restrict, size_t, struct hostent **__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int gethostbyname_r(const char *__restrict, struct hostent *__restrict, char *__restrict, size_t,
					struct hostent **__restrict, int *__restrict) {
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

struct servent *getservbyname(const char *name, const char *proto) {
	int iproto = -1;
	if (proto &&(!strncmp(proto, "tcp", 3) || !strncmp(proto, "TCP", 3)))
		iproto = IPPROTO_TCP;
	else if (proto && (!strncmp(proto, "udp", 3) || !strncmp(proto, "UDP", 3)))
		iproto = IPPROTO_UDP;

	static struct servent ret;
	if (ret.s_name) {
		free(ret.s_name);
		ret.s_name = nullptr;

		for (char **alias = ret.s_aliases; *alias != NULL; alias++) {
			free(*alias);
			*alias = nullptr;
		}

		free(ret.s_proto);
		ret.s_proto = nullptr;
	}

	mlibc::service_result serv_buf{getAllocator()};
	int count = mlibc::lookup_serv_by_name(serv_buf, name, iproto,
			0, 0);
	if (count <= 0)
		return NULL;

	ret.s_name = serv_buf[0].name.data();
	serv_buf[0].name.detach();
	// Sanity check.
	if (strncmp(name, serv_buf[0].name.data(), serv_buf[0].name.size()))
		return NULL;

	ret.s_aliases = reinterpret_cast<char**>(malloc((serv_buf[0].aliases.size() + 1) * sizeof(char*)));
	int alias_pos = 0;
	for (auto &buf_name : serv_buf[0].aliases) {
		ret.s_aliases[alias_pos] = buf_name.data();
		buf_name.detach();
		alias_pos++;
	}
	ret.s_aliases[alias_pos] = NULL;

	ret.s_port = htons(serv_buf[0].port);

	auto proto_string = frg::string<MemoryAllocator>(getAllocator());
	if (!proto) {
		if (serv_buf[0].protocol == IPPROTO_TCP)
			proto_string = frg::string<MemoryAllocator>("tcp", getAllocator());
		else if (serv_buf[0].protocol == IPPROTO_UDP)
			proto_string = frg::string<MemoryAllocator>("udp", getAllocator());
		else
			return NULL;
	} else {
		proto_string = frg::string<MemoryAllocator>(proto, getAllocator());
	}
	ret.s_proto = proto_string.data();
	proto_string.detach();

	return &ret;
}

struct servent *getservbyport(int port, const char *proto) {
	int iproto = -1;
	if (proto && (!strncmp(proto, "tcp", 3) || !strncmp(proto, "TCP", 3)))
		iproto = IPPROTO_TCP;
	else if (proto && (!strncmp(proto, "udp", 3) || !strncmp(proto, "UDP", 3)))
		iproto = IPPROTO_UDP;

	static struct servent ret;
	if (ret.s_name) {
		free(ret.s_name);
		ret.s_name = nullptr;

		for (char **alias = ret.s_aliases; *alias != NULL; alias++) {
			free(*alias);
			*alias = nullptr;
		}

		free(ret.s_proto);
		ret.s_proto = nullptr;
	}

	mlibc::service_result serv_buf{getAllocator()};
	int count = mlibc::lookup_serv_by_port(serv_buf, iproto, ntohs(port));
	if (count <= 0)
		return NULL;

	ret.s_name = serv_buf[0].name.data();
	serv_buf[0].name.detach();

	ret.s_aliases = reinterpret_cast<char**>(malloc((serv_buf[0].aliases.size() + 1) * sizeof(char*)));
	int alias_pos = 0;
	for (auto &buf_name : serv_buf[0].aliases) {
		ret.s_aliases[alias_pos] = buf_name.data();
		buf_name.detach();
		alias_pos++;
	}
	ret.s_aliases[alias_pos] = NULL;

	ret.s_port = port;

	auto proto_string = frg::string<MemoryAllocator>(getAllocator());
	if (!proto) {
		if (serv_buf[0].protocol == IPPROTO_TCP)
			proto_string = frg::string<MemoryAllocator>("tcp", getAllocator());
		else if (serv_buf[0].protocol == IPPROTO_UDP)
			proto_string = frg::string<MemoryAllocator>("udp", getAllocator());
		else
			return NULL;
	} else {
		proto_string = frg::string<MemoryAllocator>(proto, getAllocator());
	}
	ret.s_proto = proto_string.data();
	proto_string.detach();

	return &ret;
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

const char *hstrerror(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
