
#include <poll.h>
#include <mlibc/ensure.h>

int poll(struct pollfd *, nfds_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

