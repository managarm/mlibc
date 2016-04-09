
#include <sys/socket.h>

#include <mlibc/ensure.h>

int accept(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int bind(int, const struct sockaddr *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int connect(int, const struct sockaddr *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

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
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int socket(int, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int socketpair(int, int, int, int [2]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

