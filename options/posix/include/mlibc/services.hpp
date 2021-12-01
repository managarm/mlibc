#ifndef _MLIBC_SERVICES
#define _MLIBC_SERVICES

#include <frg/small_vector.hpp>
#include <frg/vector.hpp>
#include <frg/string.hpp>
#include <mlibc/allocator.hpp>

namespace mlibc {

// Only two services for tcp and udp
#define SERV_MAX 2

struct service_buf {
	service_buf()
	: name(getAllocator()), aliases(getAllocator())
	{ }
	int port, protocol, socktype;
	frg::string<MemoryAllocator> name;
	frg::vector<frg::string<MemoryAllocator>, MemoryAllocator> aliases;
};

using service_result = frg::small_vector<service_buf, SERV_MAX, MemoryAllocator>;

int lookup_serv_by_name(service_result &buf, const char *name, int proto,
		int socktype, int flags);

int lookup_serv_by_port(service_result &buf, int proto, int port);


} // namespace mlibc

#endif // _MLIBC_SERVICES
