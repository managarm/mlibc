#include <lemon/syscall.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>

namespace mlibc{
    int sys_socket(int domain, int type, int protocol, int *fd){
        long ret = syscall(SYS_SOCKET, domain, type, protocol, 0, 0);

        if(ret < 0){
            return ret;
        }

        *fd = ret;

        return 0;
    }

    int sys_bind(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
        return syscall(SYS_BIND, sockfd, addr_ptr, addrlen, 0, 0);
    }

    int sys_connect(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
        return syscall(SYS_CONNECT, sockfd, addr_ptr, addrlen, 0, 0);
    }

    int sys_accept(int fd, int *newfd){
        long ret = syscall(SYS_ACCEPT, fd, 0, 0, 0, 0);

        if(ret < 0){
            return ret;
        }

        *newfd = ret;

        return 0;
    }

    int sys_listen(int fd, int backlog){
        return syscall(SYS_LISTEN, fd, backlog, 0, 0, 0);
    }

    int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length){
        long ret = syscall(SYS_RECVMSG, sockfd, hdr, flags, 0, 0);

        if(ret < 0){
            return ret;
        }

        *length = ret;

        return 0;
    }

    int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length){
        long ret = syscall(SYS_SENDMSG, sockfd, hdr, flags, 0, 0);

        if(ret < 0){
            return ret;
        }

        *length = ret;

        return 0;
    }
}
