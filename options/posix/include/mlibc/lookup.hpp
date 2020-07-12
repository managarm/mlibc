#ifndef _MLIBC_LOOKUP
#define _MLIBC_LOOKUP

#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <frg/string.hpp>
#include <mlibc/allocator.hpp>

// Constant taken from musl
#define NAME_MAX 48

namespace mlibc {

struct dns_addr_buf {
	dns_addr_buf() : name(getAllocator())
	{}
	frg::string<MemoryAllocator> name;
	int family;
	uint8_t addr[16];
};

struct dns_header {
	uint16_t identification;
	uint16_t flags;
	uint16_t no_q;
	uint16_t no_ans;
	uint16_t no_auths;
	uint16_t no_additional;
};

struct ai_buf {
	struct addrinfo ai;
	union sa {
		struct sockaddr_in sin;
		struct sockaddr_in6 sin6;
	} sa;
};

int lookup_name_dns(struct dns_addr_buf *buf, const char *name);

} // namespace mlibc

#endif // _MLIBC_LOOKUP
