#ifndef _MLIBC_LOOKUP
#define _MLIBC_LOOKUP

#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>

namespace mlibc {

struct dns_addr_buf {
	dns_addr_buf()
	: name(getAllocator()) {}
	frg::string<MemoryAllocator> name;
	int family;
	uint8_t addr[16];
};

struct lookup_result {
	lookup_result()
	: buf(getAllocator()), aliases(getAllocator()) {}
	frg::vector<struct dns_addr_buf, MemoryAllocator> buf;
	frg::vector<frg::string<MemoryAllocator>, MemoryAllocator> aliases;
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

int lookup_name_dns(struct lookup_result &buf, const char *name,
		frg::string<MemoryAllocator> &canon_name);
int lookup_name_hosts(struct lookup_result &buf, const char *name,
		frg::string<MemoryAllocator> &canon_name);

} // namespace mlibc

#endif // _MLIBC_LOOKUP
