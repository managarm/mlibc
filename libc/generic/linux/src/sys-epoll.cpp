
#include <sys/epoll.h>

#include <mlibc/ensure.h>

int epoll_create(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int epoll_create1(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int epoll_ctl(int, int, int, struct epoll_event *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int epoll_wait(int, struct epoll_event *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int epoll_pwait(int, struct epoll_event *, int, int, const sigset_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

