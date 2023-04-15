#include <sys/types.h>

#include <keyronex/syscall.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/linux-sysdeps.hpp>

namespace mlibc {

int
sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout,
    const sigset_t *sigmask, int *raised)
{
	uintptr_t ret = syscall5(kPXSysEPollWait, epfd, (uintptr_t)ev, n,
	    timeout, (uintptr_t)sigmask, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*raised = ret;
	return 0;
}

int
sys_epoll_create(int flags, int *fd)
{
	uintptr_t ret = syscall1(kPXSysEPollCreate, flags, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*fd = ret;
	return 0;
}

int
sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev)
{
	uintptr_t ret = syscall4(kPXSysEPollCtl, epfd, mode, fd, (uintptr_t)ev,
	    NULL);
	return sc_error(ret);
}

}