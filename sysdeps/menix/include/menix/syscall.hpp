#ifndef _MENIX_SYSCALL_HPP
#define _MENIX_SYSCALL_HPP

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
#define SYSCALL_READ 24
#define SYSCALL_PREAD 25
#define SYSCALL_WRITE 26
#define SYSCALL_PWRITE 27
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
#define SYSCALL_PSELECT 41
#define SYSCALL_MKNODAT 42
#define SYSCALL_READDIR 43
#define SYSCALL_GETCWD 44
#define SYSCALL_CHDIR 45
#define SYSCALL_FCHDIR 46
#define SYSCALL_MKDIRAT 47
#define SYSCALL_RMDIRAT 48
#define SYSCALL_GETDENTS 49
#define SYSCALL_RENAMEAT 50
#define SYSCALL_FCHMOD 51
#define SYSCALL_FCHMODAT 52
#define SYSCALL_FCHOWNAT 53
#define SYSCALL_LINKAT 54
#define SYSCALL_SYMLINKAT 55
#define SYSCALL_UNLINKAT 56
#define SYSCALL_READLINKAT 57
#define SYSCALL_FLOCK 58
#define SYSCALL_PPOLL 59
#define SYSCALL_DUP 60
#define SYSCALL_DUP3 61
#define SYSCALL_SYNC 62
#define SYSCALL_FSYNC 63
#define SYSCALL_FDATASYNC 64
#define SYSCALL_GETGROUPS 65
#define SYSCALL_SETGROUPS 66
#define SYSCALL_GETSID 67
#define SYSCALL_SETSID 68
#define SYSCALL_SETUID 69
#define SYSCALL_GETUID 70
#define SYSCALL_SETGID 71
#define SYSCALL_GETGID 72
#define SYSCALL_GETEUID 73
#define SYSCALL_SETEUID 74
#define SYSCALL_GETEGID 75
#define SYSCALL_SETEGID 76
#define SYSCALL_GETPGID 77
#define SYSCALL_SETPGID 78
#define SYSCALL_GETRESUID 79
#define SYSCALL_SETRESUID 80
#define SYSCALL_GETRESGID 81
#define SYSCALL_SETRESGID 82
#define SYSCALL_SETREUID 83
#define SYSCALL_SETREGID 84
#define SYSCALL_UMASK 85
#define SYSCALL_PIPE 86
#define SYSCALL_FUTEX_WAIT 87
#define SYSCALL_FUTEX_WAKE 88
#define SYSCALL_THREAD_CREATE 89
#define SYSCALL_THREAD_KILL 90
#define SYSCALL_THREAD_EXIT 91
#define SYSCALL_THREAD_SETNAME 92
#define SYSCALL_THREAD_GETNAME 93
#define SYSCALL_TIMER_CREATE 94
#define SYSCALL_TIMER_SET 95
#define SYSCALL_TIMER_DELETE 96
#define SYSCALL_ITIMER_GET 97
#define SYSCALL_ITIMER_SET 98
#define SYSCALL_CLOCK_GET 99
#define SYSCALL_CLOCK_GETRES 100
#define SYSCALL_SLEEP 101
#define SYSCALL_YIELD 102
#define SYSCALL_CHROOT 103
#define SYSCALL_MOUNT 104
#define SYSCALL_UMOUNT 105
#define SYSCALL_SWAPON 106
#define SYSCALL_SWAPOFF 107
#define SYSCALL_SOCKET 108
#define SYSCALL_SOCKETPAIR 109
#define SYSCALL_SHUTDOWN 110
#define SYSCALL_BIND 111
#define SYSCALL_CONNECT 112
#define SYSCALL_ACCEPT 113
#define SYSCALL_LISTEN 114
#define SYSCALL_GETPEERNAME 115
#define SYSCALL_GETSOCKNAME 116
#define SYSCALL_GETSOCKOPT 117
#define SYSCALL_SETSOCKOPT 118
#define SYSCALL_SENDMSG 119
#define SYSCALL_SENDTO 120
#define SYSCALL_RECVMSG 121
#define SYSCALL_RECVFROM 122
#define SYSCALL_GETENTROPY 123
#define SYSCALL_GETRUSAGE 124
#define SYSCALL_GETRLIMIT 125
#define SYSCALL_SETRLIMIT 126
#define SYSCALL_GETPRIORITY 127
#define SYSCALL_SETPRIORITY 128
#define SYSCALL_SCHED_GETPARAM 129
#define SYSCALL_SCHED_SETPARAM 130

#if defined(__x86_64__)
#define MENIX_ASM_REG_NUM "rax"
#define MENIX_ASM_REG_RET "rax"
#define MENIX_ASM_REG_ERR "rdx"
#define MENIX_ASM_REG_A0 "rdi"
#define MENIX_ASM_REG_A1 "rsi"
#define MENIX_ASM_REG_A2 "rdx"
#define MENIX_ASM_REG_A3 "r10"
#define MENIX_ASM_REG_A4 "r8"
#define MENIX_ASM_REG_A5 "r9"
#define MENIX_ASM_SYSCALL "syscall"
#define MENIX_ASM_CLOBBER "memory", "rcx", "r11"
#elif defined(__aarch64__)
#define MENIX_ASM_REG_NUM "x8"
#define MENIX_ASM_REG_RET "x0"
#define MENIX_ASM_REG_ERR "x1"
#define MENIX_ASM_REG_A0 "x0"
#define MENIX_ASM_REG_A1 "x1"
#define MENIX_ASM_REG_A2 "x2"
#define MENIX_ASM_REG_A3 "x3"
#define MENIX_ASM_REG_A4 "x4"
#define MENIX_ASM_REG_A5 "x5"
#define MENIX_ASM_SYSCALL "svc 0"
#define MENIX_ASM_CLOBBER "memory"
#elif defined(__riscv) && (__riscv_xlen == 64)
#define MENIX_ASM_REG_NUM "a7"
#define MENIX_ASM_REG_RET "a0"
#define MENIX_ASM_REG_ERR "a1"
#define MENIX_ASM_REG_A0 "a0"
#define MENIX_ASM_REG_A1 "a1"
#define MENIX_ASM_REG_A2 "a2"
#define MENIX_ASM_REG_A3 "a3"
#define MENIX_ASM_REG_A4 "a4"
#define MENIX_ASM_REG_A5 "a5"
#define MENIX_ASM_SYSCALL "ecall"
#define MENIX_ASM_CLOBBER "memory"
#elif defined(__loongarch64)
#define MENIX_ASM_REG_NUM "a7"
#define MENIX_ASM_REG_RET "a0"
#define MENIX_ASM_REG_ERR "a1"
#define MENIX_ASM_REG_A0 "a0"
#define MENIX_ASM_REG_A1 "a1"
#define MENIX_ASM_REG_A2 "a2"
#define MENIX_ASM_REG_A3 "a3"
#define MENIX_ASM_REG_A4 "a4"
#define MENIX_ASM_REG_A5 "a5"
#define MENIX_ASM_SYSCALL "syscall 0"
#define MENIX_ASM_CLOBBER "memory"
#else
#error "Unsupported architecture!"
#endif

struct syscall_result {
	size_t value;
	size_t error;
};
static_assert(sizeof(syscall_result) == 16);

#ifndef __MLIBC_ABI_ONLY

static inline syscall_result menix_syscall(size_t num) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	asm volatile(MENIX_ASM_SYSCALL : "=r"(value), "=r"(err) : "r"(rnum) : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result menix_syscall(size_t num, size_t a0) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result menix_syscall(size_t num, size_t a0, size_t a1) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	register size_t r1 asm(MENIX_ASM_REG_A1) = a1;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result menix_syscall(size_t num, size_t a0, size_t a1, size_t a2) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	register size_t r1 asm(MENIX_ASM_REG_A1) = a1;
	register size_t r2 asm(MENIX_ASM_REG_A2) = a2;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result menix_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	register size_t r1 asm(MENIX_ASM_REG_A1) = a1;
	register size_t r2 asm(MENIX_ASM_REG_A2) = a2;
	register size_t r3 asm(MENIX_ASM_REG_A3) = a3;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result
menix_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3, size_t a4) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	register size_t r1 asm(MENIX_ASM_REG_A1) = a1;
	register size_t r2 asm(MENIX_ASM_REG_A2) = a2;
	register size_t r3 asm(MENIX_ASM_REG_A3) = a3;
	register size_t r4 asm(MENIX_ASM_REG_A4) = a4;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

static inline syscall_result
menix_syscall(size_t num, size_t a0, size_t a1, size_t a2, size_t a3, size_t a4, size_t a5) {
	register size_t rnum asm(MENIX_ASM_REG_NUM) = num;
	register size_t value asm(MENIX_ASM_REG_RET);
	register size_t err asm(MENIX_ASM_REG_ERR);
	register size_t r0 asm(MENIX_ASM_REG_A0) = a0;
	register size_t r1 asm(MENIX_ASM_REG_A1) = a1;
	register size_t r2 asm(MENIX_ASM_REG_A2) = a2;
	register size_t r3 asm(MENIX_ASM_REG_A3) = a3;
	register size_t r4 asm(MENIX_ASM_REG_A4) = a4;
	register size_t r5 asm(MENIX_ASM_REG_A5) = a5;
	asm volatile(MENIX_ASM_SYSCALL
	             : "=r"(value), "=r"(err)
	             : "r"(rnum), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
	             : MENIX_ASM_CLOBBER);

	return {value, err};
}

#endif /* !__MLIBC_ABI_ONLY */

#endif /* _MENIX_SYSCALL_HPP */
