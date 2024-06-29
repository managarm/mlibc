#ifndef _POLARIS__SYSCALL_H
#define _POLARIS__SYSCALL_H

#include <stdint.h>

#define SYS_read 0
#define SYS_write 1
#define SYS_open 2
#define SYS_close 3
#define SYS_seek 8
#define SYS_mmap 9
#define SYS_munmap 11
#define SYS_ioctl 16
#define SYS_nanosleep 35
#define SYS_getpid 39
#define SYS_fork 57
#define SYS_execve 59
#define SYS_exit 60
#define SYS_kill 62
#define SYS_fcntl 72
#define SYS_getcwd 79
#define SYS_chdir 80
#define SYS_readdir 89
#define SYS_puts 103
#define SYS_getppid 110
#define SYS_waitpid 114
#define SYS_prctl 157
#define SYS_futex 202
#define SYS_openat 257
#define SYS_mkdirat 258
#define SYS_fstatat 262
#define SYS_unlinkat 263
#define SYS_linkat 265
#define SYS_readlinkat 267
#define SYS_fchmodat 268
#define SYS_dup3 292
#define SYS_pipe 293
#define SYS_getclock 314
#define SYS_socket 41
#define SYS_connect 42
#define SYS_accept 43
#define SYS_recvmsg 47
#define SYS_bind 49
#define SYS_listen 50
#define SYS_getpeername 52
#define SYS_socketpair 83

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
}

// Cast to the argument type of the extern "C" functions.
__attribute__((always_inline)) inline sc_word_t sc_cast(long x) { return x; }
__attribute__((always_inline)) inline sc_word_t sc_cast(const void *x) {
    return reinterpret_cast<sc_word_t>(x);
}

// C++ wrappers for the extern "C" functions.
__attribute__((always_inline)) static inline long _syscall(int call) {
    return syscall0(call);
}

__attribute__((always_inline)) static inline long _syscall(int call,
                                                           sc_word_t arg0) {
    return syscall1(call, arg0);
}

__attribute__((always_inline)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1) {
    return syscall2(call, arg0, arg1);
}

__attribute__((always_inline)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2) {
    return syscall3(call, arg0, arg1, arg2);
}

__attribute__((always_inline)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3) {
    return syscall4(call, arg0, arg1, arg2, arg3);
}

__attribute__((always_inline)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3, sc_word_t arg4) {
    return syscall5(call, arg0, arg1, arg2, arg3, arg4);
}

__attribute__((always_inline)) static inline long
_syscall(int call, sc_word_t arg0, sc_word_t arg1, sc_word_t arg2,
         sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
    return syscall6(call, arg0, arg1, arg2, arg3, arg4, arg5);
}

template <typename... T>
__attribute__((always_inline)) static inline long syscall(sc_word_t call,
                                                          T... args) {
    return _syscall(call, sc_cast(args)...);
}

#endif
