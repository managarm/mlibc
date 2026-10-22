#include <sys/syscall.h>
#include <bits/syscall.h>

using sc_word_t = __sc_word_t;

sc_word_t __do_syscall0(long sc) {
    register long v0 asm("v0") = sc;
    register long a3 asm("a3");
    asm volatile ("syscall"
        : "+r"(v0), "=r"(a3)
        :
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall1(long sc,
        sc_word_t arg1) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a3 asm("a3");
    asm volatile ("syscall"
        : "+r"(v0), "=r"(a3)
        : "r"(a0)
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall2(long sc,
        sc_word_t arg1, sc_word_t arg2) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a1 asm("a1") = arg2;
    register long a3 asm("a3");
    asm volatile ("syscall"
        : "+r"(v0), "=r"(a3)
        : "r"(a0), "r"(a1)
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall3(long sc,
        sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a1 asm("a1") = arg2;
    register long a2 asm("a2") = arg3;
    register long a3 asm("a3");
    asm volatile ("syscall"
        : "+r"(v0), "=r"(a3)
        : "r"(a0), "r"(a1), "r"(a2)
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall4(long sc,
        sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
        sc_word_t arg4) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a1 asm("a1") = arg2;
    register long a2 asm("a2") = arg3;
    register long a3 asm("a3") = arg4;
    asm volatile ("syscall"
        : "+r"(v0), "+r"(a3)
        : "r"(a0), "r"(a1), "r"(a2)
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall5(long sc,
        sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
        sc_word_t arg4, sc_word_t arg5) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a1 asm("a1") = arg2;
    register long a2 asm("a2") = arg3;
    register long a3 asm("a3") = arg4;
    register long a4 asm("8") = arg5;
    asm volatile ("syscall"
        : "+r"(v0), "+r"(a3)
        : "r"(a0), "r"(a1), "r"(a2), "r"(a4)
        : "memory");
    return a3 ? -v0 : v0;
}

sc_word_t __do_syscall6(long sc,
        sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
        sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
    register long v0 asm("v0") = sc;
    register long a0 asm("a0") = arg1;
    register long a1 asm("a1") = arg2;
    register long a2 asm("a2") = arg3;
    register long a3 asm("a3") = arg4;
    register long a4 asm("8") = arg5;
    register long a5 asm("9") = arg6;
    asm volatile ("syscall"
        : "+r"(v0), "+r"(a3)
        : "r"(a0), "r"(a1), "r"(a2), "r"(a4), "r"(a5)
        : "memory");
    return a3 ? -v0 : v0;
}