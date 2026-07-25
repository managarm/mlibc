#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

namespace mlibc {

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *argument, int *output) {
	auto result = roxy_syscall3(
	    ROXY_SYS_IOCTL,
	    fd,
	    static_cast<long>(request),
	    reinterpret_cast<long>(argument)
	);
	if(result < 0)
		return static_cast<int>(-result);

	if(output)
		*output = static_cast<int>(result);
	return 0;
}

} // namespace mlibc
