
#include <sys/select.h>

#include <mlibc/ensure.h>

void FD_CLR(int fd, fd_set *set_ptr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int FD_ISSET(int fd, fd_set *set_ptr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void FD_SET(int fd, fd_set *set_ptr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void FD_ZERO(fd_set *set_ptr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
		struct timeval *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

