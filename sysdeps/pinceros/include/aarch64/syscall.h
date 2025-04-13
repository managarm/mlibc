#ifndef _SYSCALL_H
#define _SYSCALL_H

#define SHUTDOWN 1
#define YIELD 3
#define SPAWN 5
#define EXIT 6
#define CHANNEL 7
#define SEND 8
#define RECV 9
#define PREAD 10
#define PWRITE 11
#define CLOSE 12
#define DUP3 13
#define PIPE 14
#define OPENAT 15
#define EXECVE 16
#define WAIT 17
#define MMAP 18
#define MUNMAP 19

#define STR(x) #x

#define __do_syscall0(sc)            \
    ({                               \
        register long ret asm("x0"); \
        asm volatile(                \
            "svc " STR(sc)           \
            : "=r"(ret)              \
            :                        \
            : "memory");             \
        ret;                         \
    })

#define __do_syscall1(sc, arg1)                        \
    ({                                                 \
        register long arg1_reg asm("x0") = (long)arg1; \
        register long ret asm("x0");                   \
        asm volatile(                                  \
            "svc " STR(sc)                             \
            : "=r"(ret)                                \
            : "r"(arg1_reg)                            \
            : "memory");                               \
        ret;                                           \
    })

#define __do_syscall2(sc, arg1, arg2)                   \
    ({                                                  \
        long _arg1 = (long)arg1;                        \
        long _arg2 = (long)arg2;                        \
        register long arg1_reg asm("x0") = (long)_arg1; \
        register long arg2_reg asm("x1") = (long)_arg2; \
        register long ret asm("x0");                    \
        asm volatile(                                   \
            "svc " STR(sc)                              \
            : "=r"(ret)                                 \
            : "r"(arg1_reg), "r"(arg2_reg)              \
            : "memory");                                \
        ret;                                            \
    })

#define __do_syscall3(sc, arg1, arg2, arg3)               \
    ({                                                    \
        long _arg1 = (long)arg1;                          \
        long _arg2 = (long)arg2;                          \
        long _arg3 = (long)arg3;                          \
        register long arg1_reg asm("x0") = (long)_arg1;   \
        register long arg2_reg asm("x1") = (long)_arg2;   \
        register long arg3_reg asm("x2") = (long)_arg3;   \
        register long ret asm("x0");                      \
        asm volatile(                                     \
            "svc " STR(sc)                                \
            : "=r"(ret)                                   \
            : "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg) \
            : "memory");                                  \
        ret;                                              \
    })

#define __do_syscall4(sc, arg1, arg2, arg3, arg4)                        \
    ({                                                                   \
        long _arg1 = (long)arg1;                                         \
        long _arg2 = (long)arg2;                                         \
        long _arg3 = (long)arg3;                                         \
        long _arg4 = (long)arg4;                                         \
        register long arg1_reg asm("x0") = (long)_arg1;                  \
        register long arg2_reg asm("x1") = (long)_arg2;                  \
        register long arg3_reg asm("x2") = (long)_arg3;                  \
        register long arg4_reg asm("x3") = (long)_arg4;                  \
        register long ret asm("x0");                                     \
        asm volatile(                                                    \
            "svc " STR(sc)                                               \
            : "=r"(ret)                                                  \
            : "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg), "r"(arg4_reg) \
            : "memory");                                                 \
        ret;                                                             \
    })

#define __do_syscall5(sc, arg1, arg2, arg3, arg4, arg5)                                 \
    ({                                                                                  \
        long _arg1 = (long)arg1;                                                        \
        long _arg2 = (long)arg2;                                                        \
        long _arg3 = (long)arg3;                                                        \
        long _arg4 = (long)arg4;                                                        \
        long _arg5 = (long)arg5;                                                        \
        register long arg1_reg asm("x0") = (long)_arg1;                                 \
        register long arg2_reg asm("x1") = (long)_arg2;                                 \
        register long arg3_reg asm("x2") = (long)_arg3;                                 \
        register long arg4_reg asm("x3") = (long)_arg4;                                 \
        register long arg5_reg asm("x4") = (long)_arg5;                                 \
        register long ret asm("x0");                                                    \
        asm volatile(                                                                   \
            "svc " STR(sc)                                                              \
            : "=r"(ret)                                                                 \
            : "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg), "r"(arg4_reg), "r"(arg5_reg) \
            : "memory");                                                                \
        ret;                                                                            \
    })

#define __do_syscall6(sc, arg1, arg2, arg3, arg4, arg5, arg6)                            \
    ({                                                                                   \
        long _arg1 = (long)arg1;                                                         \
        long _arg2 = (long)arg2;                                                         \
        long _arg3 = (long)arg3;                                                         \
        long _arg4 = (long)arg4;                                                         \
        long _arg5 = (long)arg5;                                                         \
        long _arg6 = (long)arg6;                                                         \
        register long arg1_reg asm("x0") = (long)_arg1;                                  \
        register long arg2_reg asm("x1") = (long)_arg2;                                  \
        register long arg3_reg asm("x2") = (long)_arg3;                                  \
        register long arg4_reg asm("x3") = (long)_arg4;                                  \
        register long arg5_reg asm("x4") = (long)_arg5;                                  \
        register long arg6_reg asm("x5") = (long)_arg6;                                  \
        register long ret asm("x0");                                                     \
        asm volatile(                                                                    \
            "svc " STR(sc)                                                               \
            : "=r"(ret)                                                                  \
            : "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg), "r"(arg4_reg), "r"(arg5_reg), \
              "r"(arg6_reg)                                                              \
            : "memory");                                                                 \
        ret;                                                                             \
    })

#endif