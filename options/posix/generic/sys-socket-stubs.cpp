
#include <bits/ensure.h>
#include <sys/socket.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int accept(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int bind(int, const struct sockaddr *, socklen_t) {
	frigg::infoLogger() << "\e[31mmlibc: bind() is a no-op\e[39m" << frigg::endLog;
	return 0;
}
// connect() is provided by the platform

int getpeername(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsockname(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsockopt(int, int, int, void *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

ssize_t recvmsg(int, struct msghdr *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t send(int, const void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t sendmsg(int, const struct msghdr *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setsockopt(int, int, int, const void *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

// connectpair() is provided by the platform

