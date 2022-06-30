#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>

#include <aero/syscall.h>
#include <stdint.h>

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

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
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

int sys_accept(int fd, int *newfd) {
    auto result = syscall(SYS_ACCEPT, fd);

    if (result < 0) {
        return -result;
    }

    *newfd = result;
    return 0;
}
} // namespace mlibc
