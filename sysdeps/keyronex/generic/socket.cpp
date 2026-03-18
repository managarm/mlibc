#include <sys/errno.h>

#include <keyronex/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

int
Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd)
{
	int r = syscall3(SYS_socket, family, type, protocol, NULL);
	if (r < 0)
		return -r;
	*fd = r;
	return 0;
}

int
Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	int r = syscall3(SYS_bind, fd, (uintptr_t)addr, addrlen, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	int r = syscall3(SYS_connect, fd, (uintptr_t)addr, addrlen, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Listen>::operator()(int fd, int backlog)
{
	int r = syscall2(SYS_listen, fd, backlog, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr, socklen_t *addrlen,
    int flags)
{
	int r = syscall4(SYS_accept4, fd, (uintptr_t)addr,
	    (uintptr_t)addrlen, flags, NULL);
	if (r < 0)
		return -r;
	*newfd = r;
	return 0;
}

int
Sysdeps<MsgSend>::operator()(int fd, const struct msghdr *msg, int flags, ssize_t *length)
{
	int r = syscall3(SYS_sendmsg, fd, (uintptr_t)msg, flags, NULL);
	if (r < 0)
		return -r;
	*length = r;
	return 0;
}

int
Sysdeps<MsgRecv>::operator()(int fd, struct msghdr *msg, int flags, ssize_t *length)
{
	int r = syscall3(SYS_recvmsg, fd, (uintptr_t)msg, flags, NULL);
	if (r < 0)
		return -r;
	*length = r;
	return 0;
}

int
Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds)
{
	int r = syscall4(SYS_socketpair, domain, type_and_flags, proto,
	    (uintptr_t)fds, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *__restrict buffer,
    socklen_t *__restrict size)
{
	return -syscall5(SYS_getsockopt, fd, layer, number,
	    (uintptr_t)buffer, (uintptr_t)size, NULL);
}

int
Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer,
    socklen_t size)
{
	return -syscall5(SYS_setsockopt, fd, layer, number,
	    (uintptr_t)buffer, size, NULL);
}

}
