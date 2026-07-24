#include <mlibc/all-sysdeps.hpp>

#include <piggy/syscall.h>

namespace mlibc {
    int Sysdeps<Socket>::operator()(int family, int type, int protocol, int* fd) {
        long ret = syscall3(SYS_SOCKET, family, type, protocol);
        if (ret < 0) {
            return -ret;
        }

        *fd = ret;
        return 0;
    }

    int Sysdeps<Bind>::operator()(int fd, const struct sockaddr* addr_ptr, socklen_t addr_length) {
        long ret = syscall3(SYS_BIND, fd, (long) addr_ptr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Connect>::operator()(int fd, const struct sockaddr* addr_ptr, socklen_t addr_length) {
        long ret = syscall3(SYS_CONNECT, fd, (long) addr_ptr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t* addr_length, ssize_t* length) {
        (void) flags;

        long ret = syscall5(SYS_RECV, fd, (long) buffer, size, (long) sock_addr, (long) addr_length);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags, const sockaddr* sock_addr, socklen_t addr_length, ssize_t* length) {
        (void) flags;

        long ret = syscall5(SYS_SEND, fd, (long) buffer, size, (long) sock_addr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Sockname>::operator()(int fd, struct sockaddr* addr_ptr, socklen_t max_addr_length, socklen_t* actual_length) {
        long ret = syscall3(SYS_GETSOCKNAME, fd, (long) addr_ptr, max_addr_length);
        if (ret < 0) {
            return -ret;
        }

        *actual_length = ret;
        return 0;
    }

    int Sysdeps<Peername>::operator()(int fd, struct sockaddr* addr_ptr, socklen_t max_addr_length, socklen_t* actual_length) {
        long ret = syscall3(SYS_GETPEERNAME, fd, (long) addr_ptr, max_addr_length);
        if (ret < 0) {
            return -ret;
        }

        *actual_length = ret;
        return 0;
    }

    int Sysdeps<Shutdown>::operator()(int fd, int how) {
        long ret = syscall2(SYS_SHUTDOWN, fd, how);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }
} // namespace mlibc
