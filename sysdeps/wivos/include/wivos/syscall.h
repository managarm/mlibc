#ifndef SYSCALL_H
#define SYSCALL_H

#include <bits/ensure.h>
#include <stdint.h>

#define SYS_DEBUG   0x0
#define SYS_READ    0x1
#define SYS_WRITE   0x2
#define SYS_OPEN    0x3
#define SYS_CLOSE   0x4
#define SYS_ALLOC   0x5
#define SYS_SEEK    0x6
#define SYS_TCB_SET 0x7
#define SYS_FORK    0x8
#define SYS_IOCTL   0x9
#define SYS_EXECVE  0xA

// Invalid syscall used to trigger a log error in the kernel (as a hint)
// so, that we can implement the syscall in the kernel.
#define UNIMPLEMENTED(FUNCTION_NAME)                                           \
    {                                                                          \
        sys_libc_log("Unimplemented syscall: " FUNCTION_NAME);                 \
        sys_exit(1);                                                           \
        __builtin_unreachable();                                               \
    }

extern "C" {
using sc_qword_t = uint64_t;

static sc_qword_t syscall0(int sc) {
    sc_qword_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(sc) : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall1(int sc, uint64_t arg1) {
    sc_qword_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall2(int sc, sc_qword_t arg1, sc_qword_t arg2) {
    sc_qword_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall3(int sc, sc_qword_t arg1, sc_qword_t arg2,
                          sc_qword_t arg3) {
    sc_qword_t ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall4(int sc, sc_qword_t arg1, sc_qword_t arg2,
                          sc_qword_t arg3, sc_qword_t arg4) {
    sc_qword_t ret;

    register sc_qword_t arg4_reg asm("r10") = arg4;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall5(int sc, sc_qword_t arg1, sc_qword_t arg2,
                          sc_qword_t arg3, sc_qword_t arg4, sc_qword_t arg5) {
    sc_qword_t ret;

    register sc_qword_t arg4_reg asm("r10") = arg4;
    register sc_qword_t arg5_reg asm("r8") = arg5;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg),
                   "r"(arg5_reg)
                 : "rcx", "r11", "memory");
    return ret;
}

static sc_qword_t syscall6(int sc, sc_qword_t arg1, sc_qword_t arg2,
                          sc_qword_t arg3, sc_qword_t arg4, sc_qword_t arg5,
                          sc_qword_t arg6) {
    sc_qword_t ret;

    register sc_qword_t arg4_reg asm("r10") = arg4;
    register sc_qword_t arg5_reg asm("r8") = arg5;
    register sc_qword_t arg6_reg asm("r9") = arg6;

    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4_reg),
                   "r"(arg5_reg), "r"(arg6_reg)
                 : "rcx", "r11", "memory");
    return ret;
}
} // extern "C"

#endif