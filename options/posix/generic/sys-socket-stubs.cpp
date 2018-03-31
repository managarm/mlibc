
#include <bits/ensure.h>
#include <sys/socket.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int accept(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	if(addr_ptr || addr_length)
		frigg::infoLogger() << "\e[35mmlibc: accept() does not fill struct sockaddr\e[39m"
				<< frigg::endLog;
	int newfd;
	if(mlibc::sys_accept(fd, &newfd))
		return -1;
	return newfd;
}

int bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	if(mlibc::sys_bind(fd, addr_ptr, addr_len))
		return -1;
	return 0;
}

int connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	if(mlibc::sys_connect(fd, addr_ptr, addr_len))
		return -1;
	return 0;
}

int getpeername(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsockname(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	socklen_t actual_length;
	if(mlibc::sys_sockname(fd, addr_ptr, *addr_length, &actual_length))
		return -1;
	*addr_length = actual_length;
	return 0;
}

int getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	if(layer == SOL_SOCKET && number == SO_PEERCRED) {
		__ensure(*size == sizeof(struct ucred));
		frigg::infoLogger() << "\e[31mmlibc: getsockopt(SO_PEERCRED) returns all zeros\e[39m"
				<< frigg::endLog;

		struct ucred creds;
		creds.pid = 0;
		creds.uid = 0;
		creds.gid = 0;
		memcpy(buffer, &creds, sizeof(struct ucred));
		return 0;
	}else{
		frigg::panicLogger() << "\e[31mmlibc: Unexpected getsockopt() call\e[39m" << frigg::endLog;
		__builtin_unreachable();
	}
}

int listen(int, int) {
	frigg::infoLogger() << "\e[31mmlibc: listen() is a no-op\e[39m" << frigg::endLog;
	return 0;
}

ssize_t recv(int, void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t recvfrom(int, void *__restrict, size_t, int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t recvmsg(int fd, struct msghdr *hdr, int flags) {
	ssize_t length;
	if(mlibc::sys_msg_recv(fd, hdr, flags, &length))
		return -1;
	return length;
}

ssize_t send(int, const void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t sendmsg(int fd, const struct msghdr *hdr, int flags) {
	ssize_t length;
	if(mlibc::sys_msg_send(fd, hdr, flags, &length))
		return -1;
	return length;
}

ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) {
	return mlibc::sys_setsockopt(fd, layer, number, buffer, size);
}

int shutdown(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sockatmark(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int socket(int family, int type, int protocol) {
	int fd;
	if(mlibc::sys_socket(family, type, protocol, &fd))
		return -1;
	return fd;
}

int socketpair(int domain, int type, int protocol, int sv[2]) {
	if(mlibc::sys_socketpair(domain, type, protocol, sv))
		return -1;
	return 0;
}

// connectpair() is provided by the platform

