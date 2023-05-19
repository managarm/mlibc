#include <sys/errno.h>

#include <keyronex/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

#define log_unimplemented()                                     \
	mlibc::infoLogger() << "mlibc: " << __PRETTY_FUNCTION__ \
			    << " is a stub!" << frg::endlog;

namespace mlibc {

int
sys_socket(int family, int type, int protocol, int *fd)
{
	auto ret = syscall3(kPXSysSocket, family, type, protocol, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*fd = ret;
	return 0;
}

int
sys_bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	auto ret = syscall3(kPXSysBind, fd, (uintptr_t)addr, addrlen, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	auto ret = syscall3(kPXSysConnect, fd, (uintptr_t)addr, addrlen, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_listen(int fd, int backlog)
{
	auto ret = syscall2(kPXSysListen, fd, backlog, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_accept(int fd, int *newfd, struct sockaddr *addr, socklen_t *addrlen,
    int flags)
{
	auto ret = syscall4(kPXSysAccept, fd, (uintptr_t)addr,
	    (uintptr_t)addrlen, flags, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = ret;
	return 0;
}

int
sys_msg_send(int fd, const struct msghdr *msg, int flags, ssize_t *length)
{
	auto ret = syscall3(kPXSysSendMsg, fd, (uintptr_t)msg, flags, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*length = ret;
	return 0;
}

int
sys_msg_recv(int fd, struct msghdr *msg, int flags, ssize_t *length)
{
	auto ret = syscall3(kPXSysRecvMsg, fd, (uintptr_t)msg, flags, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*length = ret;
	return 0;
}

int
sys_socketpair(int domain, int type_and_flags, int proto, int *fds)
{
	auto ret = syscall4(kPXSysSocketPair, domain, type_and_flags, proto,
	    (uintptr_t)fds, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_getsockopt(int fd, int layer, int number, void *__restrict buffer,
    socklen_t *__restrict size)
{
	(void)fd;
	(void)layer;
	(void)number;
	(void)buffer;
	(void)size;
	log_unimplemented();
	return ENOSYS;
}

int
sys_setsockopt(int fd, int layer, int number, const void *buffer,
    socklen_t size)
{
	(void)fd;
	(void)layer;
	(void)number;
	(void)buffer;
	(void)size;
	log_unimplemented();
	return ENOSYS;
}

}
