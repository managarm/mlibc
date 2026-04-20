#include <mlibc/all-sysdeps.hpp>
#include <sys/signalfd.h>
#include <zinnia/syscall.hpp>

namespace mlibc {

int Sysdeps<SignalfdCreate>::operator()(const sigset_t *mask, int flags, int *fd) {
	auto r = zinnia_syscall(SYSCALL_SIGNALFD_CREATE, (size_t)mask, flags);
	if (r.error)
		return r.error;
	*fd = r.value;
	return 0;
}

} // namespace mlibc
