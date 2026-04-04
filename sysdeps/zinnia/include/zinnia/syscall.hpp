#ifndef _ZINNIA_SYSCALL_HPP
#define _ZINNIA_SYSCALL_HPP

#include <stddef.h>

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
#define SYSCALL_EXECVE 16
#define SYSCALL_FORK 17
#define SYSCALL_KILL 18
#define SYSCALL_GETTID 19
#define SYSCALL_GETPID 20
#define SYSCALL_GETPPID 21
#define SYSCALL_WAITID 22
#define SYSCALL_WAITPID 23
#define SYSCALL_PREAD 24
#define SYSCALL_READV 25
#define SYSCALL_PWRITE 26
#define SYSCALL_WRITEV 27
#define SYSCALL_SEEK 28
#define SYSCALL_IOCTL 29
#define SYSCALL_OPENAT 30
#define SYSCALL_CLOSE 31
#define SYSCALL_FSTAT 32
#define SYSCALL_FSTATAT 33
#define SYSCALL_STATVFS 34
#define SYSCALL_FSTATVFS 35
#define SYSCALL_FACCESSAT 36
#define SYSCALL_FCNTL 37
#define SYSCALL_FTRUNCATE 38
#define SYSCALL_FALLOCATE 39
#define SYSCALL_UTIMENSAT 40
#define SYSCALL_MKNODAT 41
#define SYSCALL_READDIR 42
#define SYSCALL_GETCWD 43
#define SYSCALL_CHDIR 44
#define SYSCALL_FCHDIR 45
#define SYSCALL_MKDIRAT 46
#define SYSCALL_RMDIRAT 47
#define SYSCALL_GETDENTS 48
#define SYSCALL_RENAMEAT 49
#define SYSCALL_FCHMOD 50
#define SYSCALL_FCHMODAT 51
#define SYSCALL_FCHOWNAT 52
#define SYSCALL_LINKAT 53
#define SYSCALL_SYMLINKAT 54
#define SYSCALL_UNLINKAT 55
#define SYSCALL_READLINKAT 56
#define SYSCALL_FLOCK 57
#define SYSCALL_PPOLL 58
#define SYSCALL_DUP 59
#define SYSCALL_DUP3 60
#define SYSCALL_SYNC 61
#define SYSCALL_FSYNC 62
#define SYSCALL_FDATASYNC 63
#define SYSCALL_GETGROUPS 64
#define SYSCALL_SETGROUPS 65
#define SYSCALL_GETSID 66
#define SYSCALL_SETSID 67
#define SYSCALL_SETUID 68
#define SYSCALL_GETUID 69
#define SYSCALL_SETGID 70
#define SYSCALL_GETGID 71
#define SYSCALL_GETEUID 72
#define SYSCALL_SETEUID 73
#define SYSCALL_GETEGID 74
#define SYSCALL_SETEGID 75
#define SYSCALL_GETPGID 76
#define SYSCALL_SETPGID 77
#define SYSCALL_GETRESUID 78
#define SYSCALL_SETRESUID 79
#define SYSCALL_GETRESGID 80
#define SYSCALL_SETRESGID 81
#define SYSCALL_SETREUID 82
#define SYSCALL_SETREGID 83
#define SYSCALL_UMASK 84
#define SYSCALL_PIPE 85
#define SYSCALL_FUTEX_WAIT 86
#define SYSCALL_FUTEX_WAKE 87
#define SYSCALL_THREAD_CREATE 88
#define SYSCALL_THREAD_KILL 89
#define SYSCALL_THREAD_EXIT 90
#define SYSCALL_THREAD_SETNAME 91
#define SYSCALL_THREAD_GETNAME 92
#define SYSCALL_TIMER_CREATE 93
#define SYSCALL_TIMER_SET 94
#define SYSCALL_TIMER_DELETE 95
#define SYSCALL_ITIMER_GET 96
#define SYSCALL_ITIMER_SET 97
#define SYSCALL_CLOCK_GET 98
#define SYSCALL_CLOCK_GETRES 99
#define SYSCALL_SLEEP 100
#define SYSCALL_YIELD 101
#define SYSCALL_CHROOT 102
#define SYSCALL_MOUNT 103
#define SYSCALL_UMOUNT 104
#define SYSCALL_SOCKET 105
#define SYSCALL_SOCKETPAIR 106
#define SYSCALL_SHUTDOWN 107
#define SYSCALL_BIND 108
#define SYSCALL_CONNECT 109
#define SYSCALL_ACCEPT 110
#define SYSCALL_LISTEN 111
#define SYSCALL_GETPEERNAME 112
#define SYSCALL_GETSOCKNAME 113
#define SYSCALL_GETSOCKOPT 114
#define SYSCALL_SETSOCKOPT 115
#define SYSCALL_SENDMSG 116
#define SYSCALL_RECVMSG 117
#define SYSCALL_GETENTROPY 118
#define SYSCALL_GETRUSAGE 119
#define SYSCALL_GETRLIMIT 120
#define SYSCALL_SETRLIMIT 121
#define SYSCALL_GETPRIORITY 122
#define SYSCALL_SETPRIORITY 123
#define SYSCALL_SCHED_GETPARAM 124
#define SYSCALL_SCHED_SETPARAM 125
#define SYSCALL_MODULE_INSERT 126
#define SYSCALL_MODULE_REMOVE 127

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

#endif /* _ZINNIA_SYSCALL_HPP */
