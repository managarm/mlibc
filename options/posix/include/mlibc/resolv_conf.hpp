#ifndef _MLIBC_RESOLV_CONF
#define _MLIBC_RESOLV_CONF

#include <frg/string.hpp>
#include <frg/optional.hpp>
#include <mlibc/allocator.hpp>

namespace mlibc {

struct nameserver_data {
	nameserver_data()
	: name(getAllocator()) {}
	frg::string<MemoryAllocator> name;
	// for in the future we can also store options here
};

frg::optional<struct nameserver_data> get_nameserver();

} // namespace mlibc

#endif // _MLIBC_RESOLV_CONF
