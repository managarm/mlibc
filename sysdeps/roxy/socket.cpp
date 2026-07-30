#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

namespace mlibc {

int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
	auto result = roxy_syscall4(
	    ROXY_SYS_SOCKETPAIR,
	    domain,
	    type_and_flags,
	    proto,
	    reinterpret_cast<long>(fds)
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

} // namespace mlibc
