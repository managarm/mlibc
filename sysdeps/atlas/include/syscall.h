
#define SYS_EXIT 0
#define SYS_OPEN 1
#define SYS_CLOSE 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_LOG_LIBC 5
#define SYS_VM_MAP 6
#define SYS_SEEK 7
#define SYS_TCB_SET 8
#define SYS_IOCTL 9
#define SYS_FORK 10
#define SYS_WAIT 11
#define SYS_STAT 12
#define SYS_FSTAT 13
#define SYS_GETPID 14
#define SYS_DUP 15
#define SYS_DUP2 16
#define SYS_READDIR 17
#define SYS_FCNTL 18
#define SYS_POLL 19
#define SYS_EXEC 20
#define SYS_CHDIR 21
#define SYS_GETCWD 22
#define SYS_MKDIR 23
#define SYS_ACCESS 24
#define SYS_CLOCK 25

#define SYSCALL_NA0(call)                                                      \
  ({                                                                           \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call)                                                   \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA1(call, arg0)                                                \
  ({                                                                           \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0)                                        \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA2(call, arg0, arg1)                                          \
  ({                                                                           \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0), "S"(arg1)                             \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA3(call, arg0, arg1, arg2)                                    \
  ({                                                                           \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2)                  \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA4(call, arg0, arg1, arg2, arg3)                              \
  ({                                                                           \
    register auto r10 asm("r10") = (arg3);                                     \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(r10)        \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA5(call, arg0, arg1, arg2, arg3, arg4)                        \
  ({                                                                           \
    register auto r10 asm("r10") = (arg3);                                     \
    register auto r9 asm("r9") = (arg4);                                       \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(r10),       \
                   "r"(r9)                                                     \
                 : "rcx", "r11", "memory");                                    \
  });

#define SYSCALL_NA6(call, arg0, arg1, arg2, arg3, arg4, arg5)                  \
  ({                                                                           \
    register auto r10 asm("r10") = (arg3);                                     \
    register auto r9 asm("r9") = (arg4);                                       \
    register auto r8 asm("r8") = (arg5);                                       \
    asm volatile("syscall"                                                     \
                 : "=a"(ret), "=d"(errno)                                      \
                 : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(r10),       \
                   "r"(r9), "r"(r8)                                            \
                 : "rcx", "r11", "memory");                                    \
  });
