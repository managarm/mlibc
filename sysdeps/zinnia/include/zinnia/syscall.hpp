#ifndef _ZINNIA_SYSCALL_HPP
#define _ZINNIA_SYSCALL_HPP

#ifndef __ASSEMBLER__
#include <stddef.h>
#endif

#define SYSCALL_EXIT 0
#define SYSCALL_SYSLOG 1
#define SYSCALL_GETUNAME 2
#define SYSCALL_SETUNAME 3
#define SYSCALL_ARCHCTL 4
#define SYSCALL_REBOOT 5
#define SYSCALL_MMAP 6
#define SYSCALL_MUNMAP 7
#define SYSCALL_MPROTECT 8
#define SYSCALL_MADVISE 9
#define SYSCALL_SIGPROCMASK 10
#define SYSCALL_SIGSUSPEND 11
#define SYSCALL_SIGPENDING 12
#define SYSCALL_SIGACTION 13
#define SYSCALL_SIGTIMEDWAIT 14
#define SYSCALL_SIGALTSTACK 15
#define SYSCALL_SIGRETURN 16
#define SYSCALL_SIGQUEUE 17
#define SYSCALL_EXECVE 18
#define SYSCALL_FEXECVE 19
#define SYSCALL_FORK 20
#define SYSCALL_KILL 21
#define SYSCALL_GETTID 22
#define SYSCALL_GETPID 23
#define SYSCALL_GETPPID 24
#define SYSCALL_WAITID 25
#define SYSCALL_WAITPID 26
#define SYSCALL_PREAD 27
#define SYSCALL_READV 28
#define SYSCALL_PWRITE 29
#define SYSCALL_WRITEV 30
#define SYSCALL_SEEK 31
#define SYSCALL_IOCTL 32
#define SYSCALL_OPENAT 33
#define SYSCALL_CLOSE 34
#define SYSCALL_FSTAT 35
#define SYSCALL_FSTATAT 36
#define SYSCALL_STATVFS 37
#define SYSCALL_FSTATVFS 38
#define SYSCALL_FACCESSAT 39
#define SYSCALL_FCNTL 40
#define SYSCALL_FTRUNCATE 41
#define SYSCALL_TRUNCATE 42
#define SYSCALL_FALLOCATE 43
#define SYSCALL_FADVISE 44
#define SYSCALL_UTIMENSAT 45
#define SYSCALL_MKNODAT 46
#define SYSCALL_GETCWD 47
#define SYSCALL_CHDIR 48
#define SYSCALL_FCHDIR 49
#define SYSCALL_MKDIRAT 50
#define SYSCALL_RMDIRAT 51
#define SYSCALL_GETDENTS 52
#define SYSCALL_RENAMEAT 53
#define SYSCALL_FCHMOD 54
#define SYSCALL_FCHMODAT 55
#define SYSCALL_FCHOWNAT 56
#define SYSCALL_LINKAT 57
#define SYSCALL_SYMLINKAT 58
#define SYSCALL_UNLINKAT 59
#define SYSCALL_READLINKAT 60
#define SYSCALL_FLOCK 61
#define SYSCALL_PPOLL 62
#define SYSCALL_DUP 63
#define SYSCALL_DUP3 64
#define SYSCALL_SYNC 65
#define SYSCALL_FSYNC 66
#define SYSCALL_FDATASYNC 67
#define SYSCALL_GETGROUPS 68
#define SYSCALL_SETGROUPS 69
#define SYSCALL_GETSID 70
#define SYSCALL_SETSID 71
#define SYSCALL_SETUID 72
#define SYSCALL_GETUID 73
#define SYSCALL_SETGID 74
#define SYSCALL_GETGID 75
#define SYSCALL_GETEUID 76
#define SYSCALL_SETEUID 77
#define SYSCALL_GETEGID 78
#define SYSCALL_SETEGID 79
#define SYSCALL_GETPGID 80
#define SYSCALL_SETPGID 81
#define SYSCALL_GETRESUID 82
#define SYSCALL_SETRESUID 83
#define SYSCALL_GETRESGID 84
#define SYSCALL_SETRESGID 85
#define SYSCALL_SETREUID 86
#define SYSCALL_SETREGID 87
#define SYSCALL_UMASK 88
#define SYSCALL_PIPE 89
#define SYSCALL_FUTEX_WAIT 90
#define SYSCALL_FUTEX_WAKE 91
#define SYSCALL_THREAD_CREATE 92
#define SYSCALL_THREAD_KILL 93
#define SYSCALL_THREAD_EXIT 94
#define SYSCALL_THREAD_SETNAME 95
#define SYSCALL_THREAD_GETNAME 96
#define SYSCALL_TIMER_CREATE 97
#define SYSCALL_TIMER_SET 98
#define SYSCALL_TIMER_GET 99
#define SYSCALL_TIMER_GETOVERRUN 100
#define SYSCALL_TIMER_DELETE 101
#define SYSCALL_ITIMER_GET 102
#define SYSCALL_ITIMER_SET 103
#define SYSCALL_CLOCK_GET 104
#define SYSCALL_CLOCK_GETRES 105
#define SYSCALL_CLOCK_SET 106
#define SYSCALL_SLEEP 107
#define SYSCALL_YIELD 108
#define SYSCALL_CHROOT 109
#define SYSCALL_MOUNT 110
#define SYSCALL_UMOUNT 111
#define SYSCALL_SOCKET 112
#define SYSCALL_SOCKETPAIR 113
#define SYSCALL_SHUTDOWN 114
#define SYSCALL_BIND 115
#define SYSCALL_CONNECT 116
#define SYSCALL_ACCEPT 117
#define SYSCALL_LISTEN 118
#define SYSCALL_GETPEERNAME 119
#define SYSCALL_GETSOCKNAME 120
#define SYSCALL_GETSOCKOPT 121
#define SYSCALL_SETSOCKOPT 122
#define SYSCALL_SENDMSG 123
#define SYSCALL_RECVMSG 124
#define SYSCALL_GETENTROPY 125
#define SYSCALL_GETRUSAGE 126
#define SYSCALL_GETRLIMIT 127
#define SYSCALL_SETRLIMIT 128
#define SYSCALL_GETPRIORITY 129
#define SYSCALL_SETPRIORITY 130
#define SYSCALL_SCHED_GETPARAM 131
#define SYSCALL_SCHED_SETPARAM 132
#define SYSCALL_MODULE_INSERT 133
#define SYSCALL_MODULE_REMOVE 134
#define SYSCALL_EPOLL_CREATE 135
#define SYSCALL_EPOLL_CTL 136
#define SYSCALL_EPOLL_PWAIT 137
#define SYSCALL_TIMERFD_CREATE 138
#define SYSCALL_TIMERFD_GETTIME 139
#define SYSCALL_TIMERFD_SETTIME 140
#define SYSCALL_SIGNALFD_CREATE 141
#define SYSCALL_EVENTFD_CREATE 142
#define SYSCALL_MSYNC 143
#define SYSCALL_SYSCONF 144
#ifndef __ASSEMBLER__

#if defined(__x86_64__)
#define ZINNIA_ASM_REG_NUM "rax"
#define ZINNIA_ASM_REG_RET "rax"
#define ZINNIA_ASM_REG_ERR "rdx"
#define ZINNIA_ASM_REG_A0 "rdi"
#define ZINNIA_ASM_REG_A1 "rsi"
#define ZINNIA_ASM_REG_A2 "rdx"
#define ZINNIA_ASM_REG_A3 "r10"
#define ZINNIA_ASM_REG_A4 "r8"
#define ZINNIA_ASM_REG_A5 "r9"
#define ZINNIA_ASM_SYSCALL "syscall"
#define ZINNIA_ASM_CLOBBER "memory", "rcx", "r11"
#elif defined(__aarch64__)
#define ZINNIA_ASM_REG_NUM "x8"
#define ZINNIA_ASM_REG_RET "x0"
#define ZINNIA_ASM_REG_ERR "x1"
#define ZINNIA_ASM_REG_A0 "x0"
#define ZINNIA_ASM_REG_A1 "x1"
#define ZINNIA_ASM_REG_A2 "x2"
#define ZINNIA_ASM_REG_A3 "x3"
#define ZINNIA_ASM_REG_A4 "x4"
#define ZINNIA_ASM_REG_A5 "x5"
#define ZINNIA_ASM_SYSCALL "svc 0"
#define ZINNIA_ASM_CLOBBER "memory"
#elif defined(__riscv) && (__riscv_xlen == 64)
#define ZINNIA_ASM_REG_NUM "a7"
#define ZINNIA_ASM_REG_RET "a0"
#define ZINNIA_ASM_REG_ERR "a1"
#define ZINNIA_ASM_REG_A0 "a0"
#define ZINNIA_ASM_REG_A1 "a1"
#define ZINNIA_ASM_REG_A2 "a2"
#define ZINNIA_ASM_REG_A3 "a3"
#define ZINNIA_ASM_REG_A4 "a4"
#define ZINNIA_ASM_REG_A5 "a5"
#define ZINNIA_ASM_SYSCALL "ecall"
#define ZINNIA_ASM_CLOBBER "memory"
#elif defined(__loongarch64)
#define ZINNIA_ASM_REG_NUM "a7"
#define ZINNIA_ASM_REG_RET "a0"
#define ZINNIA_ASM_REG_ERR "a1"
#define ZINNIA_ASM_REG_A0 "a0"
#define ZINNIA_ASM_REG_A1 "a1"
#define ZINNIA_ASM_REG_A2 "a2"
#define ZINNIA_ASM_REG_A3 "a3"
#define ZINNIA_ASM_REG_A4 "a4"
#define ZINNIA_ASM_REG_A5 "a5"
#define ZINNIA_ASM_SYSCALL "syscall 0"
#define ZINNIA_ASM_CLOBBER "memory"
#else
#error "Unsupported architecture!"
#endif

struct syscall_result {
	size_t value;
	size_t error;
};
static_assert(sizeof(syscall_result) == 16);

#ifndef __MLIBC_ABI_ONLY

static inline syscall_result zinnia_syscall(size_t num) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	asm volatile(ZINNIA_ASM_SYSCALL : "=r"(value), "=r"(err) : "r"(rnum) : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result zinnia_syscall(size_t num, size_t a0) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result zinnia_syscall(size_t num, size_t a0, size_t a1) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	register size_t r1 asm(ZINNIA_ASM_REG_A1) = a1;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result zinnia_syscall(size_t num, size_t a0, size_t a1, size_t a2) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	register size_t r1 asm(ZINNIA_ASM_REG_A1) = a1;
	register size_t r2 asm(ZINNIA_ASM_REG_A2) = a2;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result
zinnia_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	register size_t r1 asm(ZINNIA_ASM_REG_A1) = a1;
	register size_t r2 asm(ZINNIA_ASM_REG_A2) = a2;
	register size_t r3 asm(ZINNIA_ASM_REG_A3) = a3;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result
zinnia_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3, size_t a4) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	register size_t r1 asm(ZINNIA_ASM_REG_A1) = a1;
	register size_t r2 asm(ZINNIA_ASM_REG_A2) = a2;
	register size_t r3 asm(ZINNIA_ASM_REG_A3) = a3;
	register size_t r4 asm(ZINNIA_ASM_REG_A4) = a4;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result
zinnia_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3, size_t a4, size_t a5) {
	register size_t rnum asm(ZINNIA_ASM_REG_NUM) = num;
	register size_t value asm(ZINNIA_ASM_REG_RET);
	register size_t err asm(ZINNIA_ASM_REG_ERR);
	register size_t r0 asm(ZINNIA_ASM_REG_A0) = a0;
	register size_t r1 asm(ZINNIA_ASM_REG_A1) = a1;
	register size_t r2 asm(ZINNIA_ASM_REG_A2) = a2;
	register size_t r3 asm(ZINNIA_ASM_REG_A3) = a3;
	register size_t r4 asm(ZINNIA_ASM_REG_A4) = a4;
	register size_t r5 asm(ZINNIA_ASM_REG_A5) = a5;
	asm volatile(ZINNIA_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
	             : ZINNIA_ASM_CLOBBER);

	return {value, err};
}

#endif /* !__MLIBC_ABI_ONLY */

#endif /* !__ASSEMBLER__ */

#endif /* _ZINNIA_SYSCALL_HPP */
