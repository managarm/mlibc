#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <mlibc/debug.hpp>
#include <abi-bits/in.h>
#include <abi-bits/errno.h>

#include <aero/syscall.h>

#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>

namespace {

int fcntl_helper(int fd, int request, int *result, ...) {
	va_list args;
	va_start(args, result);
	if(!mlibc::sys_fcntl) {
		return ENOSYS;
	}
	int ret = mlibc::sys_fcntl(fd, request, args, result);
	va_end(args);
	return ret;
}

}

namespace mlibc {
int sys_socket(int family, int type, int protocol, int *fd) {
    auto result = syscall(SYS_SOCKET, family, type, protocol);

    if (result < 0) {
        return -result;
    }

    *fd = result;
    return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    auto result = syscall(SYS_BIND, fd, addr_ptr, (sc_word_t)addr_length);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr,
                socklen_t addr_length) {
    auto result = syscall(SYS_CONNECT, fd, addr_ptr, (sc_word_t)addr_length);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_listen(int fd, int backlog) {
    auto result = syscall(SYS_LISTEN, fd, backlog);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_accept(int sockfd, int *newfd, struct sockaddr *addr_ptr,
               socklen_t *addr_length, int flags) {
    auto result = syscall(SYS_ACCEPT, sockfd, addr_ptr, addr_length);

    if (result < 0) {
        return -result;
    }

    *newfd = result;

	if(flags & SOCK_NONBLOCK) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFL, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFL, &fcntl_ret, fcntl_ret | O_NONBLOCK);
	}

	if(flags & SOCK_CLOEXEC) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFD, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFD, &fcntl_ret, fcntl_ret | FD_CLOEXEC);
	}

    return 0;
}

int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
    auto result = syscall(SYS_SOCK_SEND, fd, hdr, flags);
    if (result < 0)
        return -result;

    *length = result;
    return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *msg_hdr, int flags,
                 ssize_t *length) {
    auto result = syscall(SYS_SOCK_RECV, sockfd, msg_hdr, flags);

    if (result < 0) {
        return -result;
    }

    *length = result;
    return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
    auto result = syscall(SYS_SOCKET_PAIR, domain, type_and_flags, proto, fds);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer,
                   socklen_t *__restrict size) {
    (void)fd;
    (void)size;
    if (layer == SOL_SOCKET && number == SO_PEERCRED) {
        mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                               "and SO_PEERCRED is unimplemented\e[39m"
                            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
        mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET "
                               "and SO_SNDBUF is unimplemented\e[39m"
                            << frg::endlog;
        *(int *)buffer = 4096;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_TYPE) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is "
               "unimplemented, hardcoding SOCK_STREAM\e[39m"
            << frg::endlog;
        *(int *)buffer = SOCK_STREAM;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_ERROR) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is "
               "unimplemented, hardcoding 0\e[39m"
            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
        mlibc::infoLogger()
            << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and "
               "SO_KEEPALIVE is unimplemented, hardcoding 0\e[39m"
            << frg::endlog;
        *(int *)buffer = 0;
        return 0;
    } else {
        mlibc::panicLogger()
            << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer
            << " number: " << number << "\e[39m" << frg::endlog;
        __builtin_unreachable();
    }

    return 0;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer,
                   socklen_t size) {
    (void)fd;
    (void)buffer;
    (void)size;

    if (layer == SOL_SOCKET && number == SO_PASSCRED) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_PASSCRED) is not "
                               "implemented correctly\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_ATTACH_FILTER) is "
                               "not implemented correctly\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_RCVBUFFORCE) is not "
                               "implemented correctly\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_SNDBUF is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_KEEPALIVE is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == SOL_SOCKET && number == SO_REUSEADDR) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET "
                               "and SO_REUSEADDR is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else if (layer == AF_NETLINK && number == SO_ACCEPTCONN) {
        mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with AF_NETLINK "
                               "and SO_ACCEPTCONN is unimplemented\e[39m"
                            << frg::endlog;
        return 0;
    } else {
        mlibc::infoLogger()
            << "\e[31mmlibc: Unexpected setsockopt() call, layer: " << layer
            << " number: " << number << "\e[39m" << frg::endlog;
        return 0;
    }
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
    auto ret = syscall(SYS_GETPEERNAME, fd, addr_ptr, &max_addr_length);
	if(int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
	return 0;
}

int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
	auto ret = syscall(SYS_GETSOCKNAME, fd, addr_ptr, &max_addr_length);
	if(int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
	return 0;
}

int sys_shutdown(int sockfd, int how) {
    auto ret = syscall(SYS_SOCK_SHUTDOWN, sockfd, how);
    if(int e = sc_error(ret); e)
        return e;
    return 0;    
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
    int fd = 0;
    int r = sys_socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

    if (r)
        return r;

    struct ifreq ifr;
    strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

    r = sys_ioctl(fd, SIOCGIFINDEX, &ifr, NULL);
    close(fd);

    if (r)
        return r;

    *ret = ifr.ifr_ifindex;
    return 0;
}
} // namespace mlibc
