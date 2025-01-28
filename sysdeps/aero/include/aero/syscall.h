#ifndef SYSCALL_H
#define SYSCALL_H

#include <bits/ensure.h>
#include <stdint.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_SHUTDOWN 4
#define SYS_EXIT 5
#define SYS_FORK 6
#define SYS_REBOOT 7
#define SYS_MMAP 8
#define SYS_MUNMAP 9
#define SYS_ARCH_PRCTL 10
#define SYS_GETDENTS 11
#define SYS_GETCWD 12
#define SYS_CHDIR 13
#define SYS_MKDIR 14
#define SYS_MKDIR_AT 15
#define SYS_RMDIR 16
#define SYS_EXEC 17
#define SYS_LOG 18
#define SYS_UNAME 19
#define SYS_WAITPID 20
#define SYS_IOCTL 21
#define SYS_GETPID 22
#define SYS_SOCKET 23
#define SYS_CONNECT 24
#define SYS_BIND 25
#define SYS_LISTEN 26
#define SYS_ACCEPT 27
#define SYS_SEEK 28
#define SYS_GETTID 29
#define SYS_GETTIME 30
#define SYS_SLEEP 31
#define SYS_ACCESS 32
#define SYS_PIPE 33
#define SYS_UNLINK 34
#define SYS_GETHOSTNAME 35
#define SYS_SETHOSTNAME 36
#define SYS_INFO 37
#define SYS_CLONE 38
#define SYS_SIGRETURN 39
#define SYS_SIGACTION 40
#define SYS_SIGPROCMASK 41
#define SYS_DUP 42
#define SYS_FCNTL 43
#define SYS_DUP2 44
#define SYS_IPC_SEND 45
#define SYS_IPC_RECV 46
#define SYS_DISCOVER_ROOT 47
#define SYS_BECOME_ROOT 48
#define SYS_STAT 49
#define SYS_FSTAT 50
#define SYS_READ_LINK 51
#define SYS_EPOLL_CREATE 52
#define SYS_EPOLL_PWAIT 53
#define SYS_EPOLL_CTL 54
#define SYS_EVENT_FD 55
#define SYS_KILL 56
#define SYS_FUTEX_WAIT 57
#define SYS_FUTEX_WAKE 58
#define SYS_LINK 59
#define SYS_BACKTRACE 60
#define SYS_POLL 61
#define SYS_EXIT_THREAD 62
#define SYS_SOCK_RECV 63
#define SYS_SETITIMER 64
#define SYS_GETITIMER 65
#define SYS_GETPPID 66
#define SYS_SOCKET_PAIR 67
#define SYS_RENAME 68
#define SYS_MPROTECT 69
#define SYS_SOCK_SEND 70
#define SYS_TRACE 71
#define SYS_SETPGID 72
#define SYS_SETSID 73
#define SYS_GETPGID 74
#define SYS_SOCK_SHUTDOWN 75
#define SYS_GETPEERNAME 76
#define SYS_GETSOCKNAME 77
#define SYS_DEBUG 78
#define SYS_SETSOCKOPT 79
#define SYS_GETSOCKOPT 80
#define SYS_SYMLINK_AT 81

/* Invalid syscall used to trigger a log error in the kernel (as a hint) */
/* so, that we can implement the syscall in the kernel. */
#define UNIMPLEMENTED(FUNCTION_NAME)                                           \
    {                                                                          \
        sys_libc_log("Unimplemented syscall: " FUNCTION_NAME);                 \
        sys_exit(1);                                                           \
        __builtin_unreachable();                                               \
    }

extern "C" {
using sc_word_t = long;

static sc_word_t syscall0(int sc) {
    sc_word_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(sc) : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall1(int sc, sc_word_t arg1) {
    sc_word_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall2(int sc, sc_word_t arg1, sc_word_t arg2) {
    sc_word_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall3(int sc, sc_word_t arg1, sc_word_t arg2,
                          sc_word_t arg3) {
    sc_word_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall4(int sc, sc_word_t arg1, sc_word_t arg2,
                          sc_word_t arg3, sc_word_t arg4) {
    sc_word_t ret;

    register sc_word_t arg4_reg asm("r10") = arg4;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall5(int sc, sc_word_t arg1, sc_word_t arg2,
                          sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
    sc_word_t ret;

    register sc_word_t arg4_reg asm("r10") = arg4;
    register sc_word_t arg5_reg asm("r8") = arg5;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg),
                   "r"(arg5_reg)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_word_t syscall6(int sc, sc_word_t arg1, sc_word_t arg2,
                          sc_word_t arg3, sc_word_t arg4, sc_word_t arg5,
                          sc_word_t arg6) {
    sc_word_t ret;

    register sc_word_t arg4_reg asm("r10") = arg4;
    register sc_word_t arg5_reg asm("r8") = arg5;
    register sc_word_t arg6_reg asm("r9") = arg6;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg),
                   "r"(arg5_reg), "r"(arg6_reg)
                 : "rcx", "r11", "memory");
    return ret;
}
} /* extern "C" */

/* Cast to the argument type of the extern "C" functions. */
__attribute__((__always_inline__)) inline sc_word_t sc_cast(long x) { return x; }
__attribute__((__always_inline__)) inline sc_word_t sc_cast(const void *x) {
    return reinterpret_cast<sc_word_t>(x);
}

/* C++ wrappers for the extern "C" functions. */
__attribute__((__always_inline__)) static inline long _syscall(int call) {
    return syscall0(call);
}

__attribute__((__always_inline__)) static inline long _syscall(int call,
                                                           sc_word_t arg0) {
    return syscall1(call, arg0);
}

__attribute__((__always_inline__)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1) {
    return syscall2(call, arg0, arg1);
}

__attribute__((__always_inline__)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2) {
    return syscall3(call, arg0, arg1, arg2);
}

__attribute__((__always_inline__)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3) {
    return syscall4(call, arg0, arg1, arg2, arg3);
}

__attribute__((__always_inline__)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3, sc_word_t arg4) {
    return syscall5(call, arg0, arg1, arg2, arg3, arg4);
}

__attribute__((__always_inline__)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
    return syscall6(call, arg0, arg1, arg2, arg3, arg4, arg5);
}

template <typename... T>
__attribute__((__always_inline__)) static inline long syscall(sc_word_t call,
                                                          T... args) {
    return _syscall(call, sc_cast(args)...);
}

inline int sc_error(long ret) {
    if (ret < 0)
        return -ret;
    return 0;
}
#endif /* SYSCALL_H */
