#include <lemon/syscall.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>

namespace mlibc{

int sys_socket(int domain, int type, int protocol, int *fd){
    long ret = syscall(SYS_SOCKET, domain, type, protocol);

    if(ret < 0){
        return -ret;
    }

    *fd = ret;

    return 0;
}

int sys_bind(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
    return syscall(SYS_BIND, sockfd, addr_ptr, addrlen);
}

int sys_connect(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
    return syscall(SYS_CONNECT, sockfd, addr_ptr, addrlen);
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length){
    long ret = syscall(SYS_ACCEPT, fd);

    if(ret < 0){
        return -ret;
    }

    *newfd = ret;

    return 0;
}

int sys_listen(int fd, int backlog){
    return syscall(SYS_LISTEN, fd, backlog);
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length){
    long ret = syscall(SYS_RECVMSG, sockfd, hdr, flags);

    if(ret < 0){
        return -ret;
    }

    *length = ret;

    return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length){
    long ret = syscall(SYS_SENDMSG, sockfd, hdr, flags);

    if(ret < 0){
        return -ret;
    }

    *length = ret;

    return 0;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size){
    long ret = syscall(SYS_SET_SOCKET_OPTIONS, fd, layer, number, buffer, size);

    if(ret < 0){
        return -ret;
    }

    return 0;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size){
    long ret = syscall(SYS_GET_SOCKET_OPTIONS, fd, layer, number, buffer, size);

    if(ret < 0){
        return -ret;
    }

    return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds){
    return -syscall(SYS_SOCKETPAIR, domain, type_and_flags, proto, fds);
}

int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
    socklen_t *actual_length) {
    return -syscall(SYS_SOCKNAME, fd, addr_ptr, max_addr_length);
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
    socklen_t *actual_length) {
    return -syscall(SYS_PEERNAME, fd, addr_ptr, max_addr_length);
}

}
