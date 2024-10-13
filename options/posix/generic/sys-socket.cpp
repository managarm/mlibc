
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/socket.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int accept(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	int newfd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_accept, -1);
	if(int e = mlibc::sys_accept(fd, &newfd, addr_ptr, addr_length, 0); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int accept4(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length, int flags) {
	int newfd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_accept, -1);
	if(int e = mlibc::sys_accept(fd, &newfd, addr_ptr, addr_length, flags); e) {
		errno = e;
		return -1;
	}

	return newfd;
}

int bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_bind, -1);
	if(int e = mlibc::sys_bind(fd, addr_ptr, addr_len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_connect, -1);
	if(int e = mlibc::sys_connect(fd, addr_ptr, addr_len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getpeername(int fd, struct sockaddr *addr_ptr, socklen_t *__restrict addr_length) {
	socklen_t actual_length;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_peername, -1);
	if(int e = mlibc::sys_peername(fd, addr_ptr, *addr_length, &actual_length); e) {
		errno = e;
		return -1;
	}
	*addr_length = actual_length;
	return 0;
}

int getsockname(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	socklen_t actual_length;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sockname, -1);
	if(int e = mlibc::sys_sockname(fd, addr_ptr, *addr_length, &actual_length); e) {
		errno = e;
		return -1;
	}
	*addr_length = actual_length;
	return 0;
}

int getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getsockopt, -1);
	return mlibc::sys_getsockopt(fd, layer, number, buffer, size);
}

int listen(int fd, int backlog) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_listen, -1);
	if(int e = mlibc::sys_listen(fd, backlog); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t recv(int sockfd, void *__restrict buf, size_t len, int flags) {
	return recvfrom(sockfd, buf, len, flags, NULL, NULL);
}

ssize_t recvfrom(int sockfd, void *__restrict buf, size_t len, int flags,
		struct sockaddr *__restrict src_addr, socklen_t *__restrict addrlen) {
	if(mlibc::sys_recvfrom) {
		ssize_t length;
		if(int e = mlibc::sys_recvfrom(sockfd, buf, len, flags, src_addr, addrlen, &length); e) {
			errno = e;
			return -1;
		}
		return length;
	}

	struct iovec iov = {};
	iov.iov_base = buf;
	iov.iov_len = len;

	struct msghdr hdr = {};
	hdr.msg_name = src_addr;
	if (addrlen) {
		hdr.msg_namelen = *addrlen;
	}
	hdr.msg_iov = &iov;
	hdr.msg_iovlen = 1;

	int ret = recvmsg(sockfd, &hdr, flags);
	if (ret < 0)
		return ret;

	if(addrlen)
		*addrlen = hdr.msg_namelen;
	return ret;
}

ssize_t recvmsg(int fd, struct msghdr *hdr, int flags) {
	ssize_t length;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msg_recv, -1);
	if(int e = mlibc::sys_msg_recv(fd, hdr, flags, &length); e) {
		errno = e;
		return -1;
	}
	return length;
}

int recvmmsg(int, struct mmsghdr *, unsigned int, int, struct timespec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t send(int fd, const void *buffer, size_t size, int flags) {
	return sendto(fd, buffer, size, flags, nullptr, 0);
}

ssize_t sendto(int fd, const void *buffer, size_t size, int flags,
		const struct sockaddr *sock_addr, socklen_t addr_length) {
	if(mlibc::sys_sendto) {
		ssize_t length;
		if(int e = mlibc::sys_sendto(fd, buffer, size, flags, sock_addr, addr_length, &length); e) {
			errno = e;
			return -1;
		}
		return length;
	}

	struct iovec iov = {};
	iov.iov_base = const_cast<void *>(buffer);
	iov.iov_len = size;

	struct msghdr hdr = {};
	hdr.msg_name = const_cast<struct sockaddr *>(sock_addr);
	hdr.msg_namelen = addr_length;
	hdr.msg_iov = &iov;
	hdr.msg_iovlen = 1;

	return sendmsg(fd, &hdr, flags);
}

ssize_t sendmsg(int fd, const struct msghdr *hdr, int flags) {
	if(hdr->msg_iovlen > IOV_MAX)
		return EMSGSIZE;

	ssize_t length;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msg_send, -1);
	if(int e = mlibc::sys_msg_send(fd, hdr, flags, &length); e) {
		errno = e;
		return -1;
	}
	return length;
}

int sendmmsg(int, struct mmsghdr *, unsigned int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setsockopt, -1);
	return mlibc::sys_setsockopt(fd, layer, number, buffer, size);
}

int shutdown(int sockfd, int how) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_shutdown, -1);
	if(int e = sysdep(sockfd, how); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int sockatmark(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int socket(int family, int type, int protocol) {
	int fd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_socket, -1);
	if(int e = mlibc::sys_socket(family, type, protocol, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int socketpair(int domain, int type, int protocol, int sv[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_socketpair, -1);
	if(int e = mlibc::sys_socketpair(domain, type, protocol, sv); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// connectpair() is provided by the platform

