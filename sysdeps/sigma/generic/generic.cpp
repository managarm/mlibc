#include <mlibc/all-sysdeps.hpp>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#include <libsigma/sys.h>


#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {
    // Unknown Functions
    //int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) STUB_ONLY

    // Memory Related Functions
    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        if(!(flags & MAP_ANONYMOUS))
            __ensure(!"MAP_ANONYMOUS must be set, fd based mmaps are unimplemented");

        void* ret = libsigma_vm_map(size, hint, NULL, prot, flags);
        if(!ret)
            return ENOMEM;
        *window = ret;
        return 0;
    }

    /*#ifndef MLIBC_BUILDING_RTDL
    int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) STUB_ONLY
    #endif*/
    int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY

    // Misc functions
    #ifndef MLIBC_BUILDING_RTDL
    int sys_clock_get(int clock, time_t *secs, long *nanos) STUB_ONLY
    #endif

    // TODO: Actually implement this
    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        return 0;
    }

    int sys_futex_wake(int *pointer){
        return 0;
    }

    #ifndef MLIBC_BUILDING_RTDL
    // Task functions
    void sys_exit(int status){
        (void)(status);
        libsigma_kill();
    }

    int sys_tcb_set(void *pointer){
        libsigma_set_fsbase(reinterpret_cast<uint64_t>(pointer));
        return 0;
    }
    #endif



    /*#ifndef MLIBC_BUILDING_RTDL
    int sys_sleep(time_t *secs, long *nanos) STUB_ONLY
    int sys_fork(pid_t *child) STUB_ONLY
    int sys_execve(const char *path, char *const argv[], char *const envp[]) STUB_ONLY
    int sys_kill(pid_t pid, int signal) STUB_ONLY
    int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) STUB_ONLY
    int sys_getrusage(int who, struct rusage *usage) STUB_ONLY
    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) STUB_ONLY
    void sys_yield() STUB_ONLY
    gid_t sys_getgid() STUB_ONLY
    gid_t sys_getegid() STUB_ONLY
    uid_t sys_getuid() STUB_ONLY
    uid_t sys_geteuid() STUB_ONLY
    pid_t sys_getpid() STUB_ONLY
    pid_t sys_getppid() STUB_ONLY
    #endif*/

    // Network related functions
    #ifndef MLIBC_BUILDING_RTDL
    int sys_socket(int family, int type, int protocol, int *fd) STUB_ONLY
    int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) STUB_ONLY
    int sys_accept(int fd, int *newfd) STUB_ONLY
    int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
    int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
    int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) STUB_ONLY
    int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) STUB_ONLY
    int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) STUB_ONLY
    int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) STUB_ONLY
    int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) STUB_ONLY
    #endif*/
}
