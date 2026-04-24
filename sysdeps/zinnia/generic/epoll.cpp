#include <mlibc/all-sysdeps.hpp>
#include <sys/epoll.h>
#include <zinnia/syscall.hpp>

namespace mlibc {

int Sysdeps<EpollCreate>::operator()(int flags, int *fd) {
	auto r = zinnia_syscall(SYSCALL_EPOLL_CREATE, flags);
	if (r.error)
		return r.error;
	*fd = r.value;
	return 0;
}

int Sysdeps<EpollCtl>::operator()(int epfd, int op, int fd, struct epoll_event *event) {
	return zinnia_syscall(SYSCALL_EPOLL_CTL, epfd, op, fd, (size_t)event).error;
}

int Sysdeps<EpollPwait>::operator()(
    int epfd,
    struct epoll_event *events,
    int maxevents,
    int timeout,
    const sigset_t *sigmask,
    int *raised
) {
	auto r = zinnia_syscall(
	    SYSCALL_EPOLL_PWAIT, epfd, (size_t)events, maxevents, timeout, (size_t)sigmask
	);
	if (r.error)
		return r.error;
	*raised = r.value;
	return 0;
}

} // namespace mlibc
