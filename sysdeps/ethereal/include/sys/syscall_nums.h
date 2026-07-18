#ifndef _SYS_SYSCALL_NUMS_H
#define _SYS_SYSCALL_NUMS_H

/* System call definitions */
#define SYS_EXIT            0
#define SYS_OPEN            2
#define SYS_READ            3
#define SYS_WRITE           4
#define SYS_CLOSE           5
#define SYS_STAT            6
#define SYS_FSTAT           7
#define SYS_LSTAT           8
#define SYS_IOCTL           9
/* SYS_READDIR removed */
#define SYS_POLL            11
#define SYS_MKDIR           12
#define SYS_PSELECT         13
#define SYS_READLINK        14
#define SYS_ACCESS          15
#define SYS_CHMOD           16
#define SYS_FCNTL           17
#define SYS_UNLINKAT        18
#define SYS_FTRUNCATE       19
/* SYS_BRK removed */
#define SYS_FORK            21
#define SYS_LSEEK           22
#define SYS_GETTIMEOFDAY    23
#define SYS_SETTIMEOFDAY    24
#define SYS_USLEEP          25
#define SYS_EXECVE          26
#define SYS_WAIT            27
#define SYS_GETCWD          28
#define SYS_CHDIR           29
#define SYS_FCHDIR          30
#define SYS_UNAME           31
#define SYS_GETPID          32
#define SYS_TIMES           33
#define SYS_MMAP            34
#define SYS_MPROTECT        35
#define SYS_MUNMAP          36
#define SYS_MSYNC           37
#define SYS_DUP2            38
#define SYS_SIGNAL          39
#define SYS_SIGACTION       40
#define SYS_SIGPENDING      41
#define SYS_SIGPROCMASK     42
#define SYS_SIGSUSPEND      43
#define SYS_SIGWAIT         44
#define SYS_KILL            45
#define SYS_SOCKET          46
#define SYS_BIND            47
#define SYS_ACCEPT          48
#define SYS_LISTEN          49
#define SYS_CONNECT         50
#define SYS_GETSOCKOPT      51
#define SYS_SETSOCKOPT      52
#define SYS_SENDMSG         53
#define SYS_RECVMSG         54
#define SYS_SHUTDOWN        55
#define SYS_GETSOCKNAME     56
#define SYS_GETPEERNAME     57
#define SYS_SOCKETPAIR      58
#define SYS_MOUNT           59
#define SYS_UMOUNT          60
#define SYS_PIPE            61
#define SYS_SHARED_NEW      62  // Ethereal API
#define SYS_SHARED_KEY      63  // Ethereal API
#define SYS_SHARED_OPEN     64  // Ethereal API
#define SYS_CREATE_THREAD   65  // Ethereal API (pthread)
#define SYS_GETTID          66  // Ethereal API (pthread)
#define SYS_SETTLS          67  // Ethereal API (pthread)
#define SYS_EXIT_THREAD     68  // Ethereal API (pthread)
#define SYS_JOIN_THREAD     69  // Ethereal API (pthread)
#define SYS_KILL_THREAD     70  // Ethereal API (pthread)
#define SYS_EPOLL_CREATE    71
#define SYS_EPOLL_CTL       72
#define SYS_EPOLL_PWAIT     73
#define SYS_OPENPTY         74
#define SYS_GETUID          75
#define SYS_SETUID          76
#define SYS_GETGID          77
#define SYS_SETGID          78
#define SYS_GETPPID         79
#define SYS_GETPGID         80
#define SYS_SETPGID         81
#define SYS_GETSID          82
#define SYS_SETSID          83
#define SYS_GETEUID         84
#define SYS_SETEUID         85
#define SYS_GETEGID         86
#define SYS_SETEGID         87
#define SYS_GETHOSTNAME     88
#define SYS_SETHOSTNAME     89
#define SYS_YIELD           90
#define SYS_LOAD_DRIVER     91 // Ethereal API
#define SYS_UNLOAD_DRIVER   92 // Ethereal API
#define SYS_GET_DRIVER      93 // Ethereal API
#define SYS_SETITIMER       94
#define SYS_PTRACE          95
#define SYS_REBOOT          96 // Ethereal API
#define SYS_READ_ENTRIES    97 // mlibc
#define SYS_FUTEX_WAIT      98
#define SYS_FUTEX_WAKE      99
#define SYS_OPENAT          100
#define SYS_RENAMEAT        101
#define SYS_LINKAT          102
#define SYS_SYMLINKAT       103
#define SYS_FCHMODAT        104 // modified to accept a NULL pathname
#define SYS_MKNODAT         105
#define SYS_FLOCK           106
#define SYS_UMASK           107
#define SYS_CLOCK_GETTIME   108
#define SYS_CLOCK_SETTIME   109
#define SYS_FSYNC           110
#define SYS_PREAD           111
#define SYS_PWRITE          112
#define SYS_GETRLIMIT       113
#define SYS_SETRLIMIT       114
#define SYS_PAUSE           115
#define SYS_FCHOWNAT        116
#define SYS_FACCESSAT       117
#define SYS_SYNC            118
#define SYS_FSTATAT         119

#endif
