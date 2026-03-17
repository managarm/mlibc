
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/socket.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

static_assert(std::is_unsigned_v<sa_family_t>, "sa_family_t must be unsigned!");

int accept(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	int newfd;
	if(int e = mlibc::sysdep_or_enosys<Accept>(fd, &newfd, addr_ptr, addr_length, 0); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int accept4(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length, int flags) {
	int newfd;
	if(int e = mlibc::sysdep_or_enosys<Accept>(fd, &newfd, addr_ptr, addr_length, flags); e) {
		errno = e;
		return -1;
	}

	return newfd;
}

int bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	if(int e = mlibc::sysdep_or_enosys<Bind>(fd, addr_ptr, addr_len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_len) {
	if(int e = mlibc::sysdep_or_enosys<Connect>(fd, addr_ptr, addr_len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getpeername(int fd, struct sockaddr *addr_ptr, socklen_t *__restrict addr_length) {
	socklen_t actual_length;
	if(int e = mlibc::sysdep_or_enosys<Peername>(fd, addr_ptr, *addr_length, &actual_length); e) {
		errno = e;
		return -1;
	}
	*addr_length = actual_length;
	return 0;
}

int getsockname(int fd, struct sockaddr *__restrict addr_ptr, socklen_t *__restrict addr_length) {
	socklen_t actual_length;
	if(int e = mlibc::sysdep_or_enosys<Sockname>(fd, addr_ptr, *addr_length, &actual_length); e) {
		errno = e;
		return -1;
	}
	*addr_length = actual_length;
	return 0;
}

int getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	if (int e = mlibc::sysdep_or_enosys<GetSockopt>(fd, layer, number, buffer, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int listen(int fd, int backlog) {
	if(int e = mlibc::sysdep_or_enosys<Listen>(fd, backlog); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t recv(int sockfd, void *__restrict buf, size_t len, int flags) {
	return recvfrom(sockfd, buf, len, flags, nullptr, nullptr);
}

ssize_t recvfrom(int sockfd, void *__restrict buf, size_t len, int flags,
		struct sockaddr *__restrict src_addr, socklen_t *__restrict addrlen) {
	if constexpr (mlibc::IsImplemented<Recvfrom>) {
		ssize_t length;
		if(int e = mlibc::sysdep<Recvfrom>(sockfd, buf, len, flags, src_addr, addrlen, &length); e) {
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
	if(int e = mlibc::sysdep_or_enosys<MsgRecv>(fd, hdr, flags, &length); e) {
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
	if constexpr (mlibc::IsImplemented<Sendto>) {
		ssize_t length;
		if(int e = mlibc::sysdep_or_enosys<Sendto>(fd, buffer, size, flags, sock_addr, addr_length, &length); e) {
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
	if(int e = mlibc::sysdep_or_enosys<MsgSend>(fd, hdr, flags, &length); e) {
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
	if (int e = mlibc::sysdep_or_enosys<SetSockopt>(fd, layer, number, buffer, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int shutdown(int sockfd, int how) {
	if(int e = mlibc::sysdep_or_enosys<Shutdown>(sockfd, how); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int sockatmark(int sockfd) {
	int out = 0;
	if(int e = mlibc::sysdep_or_enosys<Sockatmark>(sockfd, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int socket(int family, int type, int protocol) {
	int fd;
	if(int e = mlibc::sysdep_or_enosys<Socket>(family, type, protocol, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int socketpair(int domain, int type, int protocol, int sv[2]) {
	if(int e = mlibc::sysdep_or_enosys<Socketpair>(domain, type, protocol, sv); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// connectpair() is provided by the platform

