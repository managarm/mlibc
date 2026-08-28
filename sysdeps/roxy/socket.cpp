#include <sys/socket.h>

#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

namespace mlibc {

int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
	auto result = roxy_syscall4(
	    ROXY_SYS_SOCKETPAIR,
	    domain,
	    type_and_flags,
	    proto,
	    reinterpret_cast<long>(fds)
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	auto result = roxy_syscall3(ROXY_SYS_SOCKET, family, type, protocol);

	if (result < 0) {
		return static_cast<int>(-result);
	}

	*fd = static_cast<int>(result);
	return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto result = roxy_syscall3(
	    ROXY_SYS_BIND,
	    fd,
	    reinterpret_cast<long>(addr_ptr),
	    addr_length
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	auto result = roxy_syscall2(ROXY_SYS_LISTEN, fd, backlog);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Accept>::operator()(
    int fd,
    int *newfd,
    struct sockaddr *addr_ptr,
    socklen_t *addr_length,
    int flags
) {
	if (flags != 0) {
		// accept4() descriptor flags are not supported by the Roxy ABI yet.
		return EINVAL;
	}

	auto result = roxy_syscall1(ROXY_SYS_ACCEPT, fd);

	if (result < 0) {
		return static_cast<int>(-result);
	}

	*newfd = static_cast<int>(result);

	// The kernel does not report peer addresses. Roxy client endpoints are always unnamed
	// AF_UNIX sockets because the kernel refuses client-side bind(), so reporting an unnamed
	// address here is exact rather than a placeholder.
	if (addr_length) {
		if (addr_ptr && *addr_length >= sizeof(sa_family_t)) {
			*reinterpret_cast<sa_family_t *>(addr_ptr) = AF_UNIX;
		}

		*addr_length = sizeof(sa_family_t);
	}

	return 0;
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto result = roxy_syscall3(
	    ROXY_SYS_CONNECT,
	    fd,
	    reinterpret_cast<long>(addr_ptr),
	    addr_length
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

} // namespace mlibc
