#include <mlibc/all-sysdeps.hpp>
#include <sys/eventfd.h>
#include <zinnia/syscall.hpp>

namespace mlibc {

int Sysdeps<EventfdCreate>::operator()(unsigned int initval, int flags, int *fd) {
	auto r = zinnia_syscall(SYSCALL_SIGNALFD_CREATE, initval, flags);
	if (r.error)
		return r.error;
	*fd = r.value;
	return 0;
}

} // namespace mlibc
