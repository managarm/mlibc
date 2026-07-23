#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <sys/syscall_nums.h>

/* System call instruction */
#define SYSCALL_INSTRUCTION "syscall"
#define SYSCALL_CLOBBERS    "rcx", "r11", "memory"

#define SYSCALL0(num) \
({ \
    long __ret = (num); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})

#define SYSCALL1(num, p1) \
({ \
    long __ret = (num); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret), "D"((long)(p1)) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})

#define SYSCALL2(num, p1, p2) \
({ \
    long __ret = (num); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret), "D"((long)(p1)), "S"((long)(p2)) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})

#define SYSCALL3(num, p1, p2, p3) \
({ \
    long __ret = (num); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret), "D"((long)(p1)), "S"((long)(p2)), "d"((long)(p3)) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})

#define SYSCALL4(num, p1, p2, p3, p4) \
({ \
    long __ret = (num); \
    register long _p4 __asm__("r10") = (long)(p4); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret), "D"((long)(p1)), "S"((long)(p2)), "d"((long)(p3)), \
          "r"(_p4) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})

#define SYSCALL5(num, p1, p2, p3, p4, p5) \
({ \
    long __ret = (num); \
    register long _p4 __asm__("r10") = (long)(p4); \
    register long _p5 __asm__("r8")  = (long)(p5); \
    asm volatile (SYSCALL_INSTRUCTION \
        : "=a"(__ret) \
        : "a"(__ret), "D"((long)(p1)), "S"((long)(p2)), "d"((long)(p3)), \
          "r"(_p4), "r"(_p5) \
        : SYSCALL_CLOBBERS); \
    __ret; \
})
    
#endif

#ifndef __sets_errno
#define __sets_errno(fn) {long _ret = fn; if ((int)_ret < 0) { errno = -_ret; _ret = -1; } return _ret; }
#endif

#ifdef __cplusplus
};
#endif
