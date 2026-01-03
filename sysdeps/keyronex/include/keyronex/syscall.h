#ifndef _KEYRONEX__SYSCALL_H
#define _KEYRONEX__SYSCALL_H

#include <stdint.h>

enum posix_syscall {
	/* processes */
	SYS_exit,
	SYS_fork,
	SYS_execve,
	SYS_wait4,
	SYS_getpid,
	SYS_getppid,

	/* job control */
	SYS_getpgid,
	SYS_setpgid,
	SYS_getsid,
	SYS_setsid,

	/* threads */
	SYS_thread_create,
	SYS_thread_exit,
	SYS_thread_gettid,
	SYS_futex_wait,
	SYS_futex_wake,
	SYS_tcb_set,
	SYS_tcb_get,

	/* signals */
	SYS_kill,
	SYS_sigaction,
	SYS_sigentry,
	SYS_sigprocmask,
	SYS_sigreturn,
	SYS_sigsuspend,

	/* vm */
	SYS_mmap,
	SYS_munmap,
	SYS_mprotect,

	/* vfs ops */
	SYS_openat,
	SYS_faccessat,
	SYS_mkdirat,
	SYS_linkat,
	SYS_unlinkat,
	SYS_renameat,
	SYS_fstatat,

	/* open file ops */
	SYS_close,
	SYS_read,
	SYS_readv,
	SYS_getdents,
	SYS_write,
	SYS_seek,
	SYS_ioctl,
	SYS_fsync,

	/* fd manipulation */
	SYS_pipe,
	SYS_dup,
	SYS_dup2,
	SYS_dup3,
	SYS_fcntl,

	/* sockets */
	SYS_socket,
	SYS_socketpair,
	SYS_bind,
	SYS_connect,
	SYS_listen,
	SYS_accept4,
	SYS_getsockopt,
	SYS_setsockopt,
	SYS_recvmsg,
	SYS_sendmsg,

	/* linux */
	SYS_epoll_create,
	SYS_epoll_ctl,
	SYS_epoll_wait,

	/* time */
	SYS_clock_gettime,
	SYS_clock_getres,
	SYS_clock_nanosleep,

	/* creds */
	SYS_getresuid,
	SYS_getresgid,

	/* misc */
	SYS_utsname,
	SYS_debug_message,
};

#if defined(__x86_64__)
static inline uintptr_t
syscall0(uintptr_t num, uintptr_t *out)
{
	uintptr_t ret, ret2;
	asm volatile("int $0x80" : "=a"(ret), "=D"(ret2) : "a"(num) : "memory");
	if (out)
		*out = ret2;
	return ret;
}

static inline uintptr_t
syscall1(uintptr_t num, uintptr_t arg1, uintptr_t *out)
{
	uintptr_t ret, ret2;
	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1)
		     : "memory");
	if (out)
		*out = ret2;
	return ret;
}

static inline uintptr_t
syscall2(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t *out)
{
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall3(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t *out)
{
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall4(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall5(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4, r8 asm("r8") = arg5;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
		     "r"(r8)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall6(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t arg6, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4, r8 asm("r8") = arg5,
			       r9 asm("r9") = arg6;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
		     "r"(r8), "r"(r9)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}
#elif defined (__m68k__)

/*
 * number: d0
 * arg 1: d1
 * arg 2: d2
 * arg 3: d3
 * arg 4: d4
 * arg 5: d5
 * arg 6: a0
 * return value: d0
 * additional output: d1
 */

static inline uintptr_t
syscall0(uintptr_t num, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1");

	asm volatile("trap #0\n\t" : "+r"(d0) : "r"(d1) : "memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall1(uintptr_t num, uintptr_t arg1, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1;

	asm volatile("trap #0\n\t" : "+r"(d0), "+r"(d1)::"memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall2(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1,
			      d2 asm("d2") = arg2;

	asm volatile("trap #0\n\t" : "+r"(d0), "+r"(d1) : "r"(d2) : "memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall3(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1,
			      d2 asm("d2") = arg2, d3 asm("d3") = arg3;

	asm volatile("trap #0\n\t"
		     : "+r"(d0), "+r"(d1)
		     : "r"(d2), "r"(d3)
		     : "memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall4(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1,
			      d2 asm("d2") = arg2, d3 asm("d3") = arg3,
			      d4 asm("d4") = arg4;

	asm volatile("trap #0\n\t"
		     : "+r"(d0), "+r"(d1)
		     : "r"(d2), "r"(d3), "r"(d4)
		     : "memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall5(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1,
			      d2 asm("d2") = arg2, d3 asm("d3") = arg3,
			      d4 asm("d4") = arg4, d5 asm("d5") = arg5;

	asm volatile("trap #0\n\t"
		     : "+r"(d0), "+r"(d1)
		     : "r"(d2), "r"(d3), "r"(d4), "r"(d5)
		     : "memory");

	if (out)
		*out = d1;

	return d0;
}

static inline uintptr_t
syscall6(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t arg6, uintptr_t *out)
{
	register uintptr_t d0 asm("d0") = num, d1 asm("d1") = arg1,
			      d2 asm("d2") = arg2, d3 asm("d3") = arg3,
			      d4 asm("d4") = arg4, d5 asm("d5") = arg5,
			      a0 asm("a0") = arg6;

	asm volatile("trap #0\n\t"
		     : "+r"(d0), "+r"(d1)
		     : "r"(d2), "r"(d3), "r"(d4), "r"(d5), "r"(a0)
		     : "memory");

	if (out)
		*out = d1;

	return d0;
}

#elif defined (__aarch64__)
/*
 * number: x0
 * arg 1: x1
 * arg 2: x2
 * arg 3: x3
 * arg 4: x4
 * arg 5: x5
 * arg 6: x6
 * return value: x0
 * additional output: x1
 */

static inline uintptr_t
syscall0(uintptr_t num, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1");

	asm volatile("svc 0\n\t" : "+r"(x0) : "r"(x1) : "memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall1(uintptr_t num, uintptr_t arg1, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1;

	asm volatile("svc 0\n\t" : "+r"(x0), "+r"(x1)::"memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall2(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1,
			      x2 asm("x2") = arg2;

	asm volatile("svc 0\n\t" : "+r"(x0), "+r"(x1) : "r"(x2) : "memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall3(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1,
			      x2 asm("x2") = arg2, x3 asm("x3") = arg3;

	asm volatile("svc 0\n\t"
		     : "+r"(x0), "+r"(x1)
		     : "r"(x2), "r"(x3)
		     : "memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall4(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1,
			      x2 asm("x2") = arg2, x3 asm("x3") = arg3,
			      x4 asm("x4") = arg4;

	asm volatile("svc 0\n\t"
		     : "+r"(x0), "+r"(x1)
		     : "r"(x2), "r"(x3), "r"(x4)
		     : "memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall5(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1,
			      x2 asm("x2") = arg2, x3 asm("x3") = arg3,
			      x4 asm("x4") = arg4, x5 asm("x5") = arg5;

	asm volatile("svc 0\n\t"
		     : "+r"(x0), "+r"(x1)
		     : "r"(x2), "r"(x3), "r"(x4), "r"(x5)
		     : "memory");

	if (out)
		*out = x1;

	return x0;
}

static inline uintptr_t
syscall6(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t arg6, uintptr_t *out)
{
	register uintptr_t x0 asm("x0") = num, x1 asm("x1") = arg1,
			      x2 asm("x2") = arg2, x3 asm("x3") = arg3,
			      x4 asm("x4") = arg4, x5 asm("x5") = arg5,
			      x6 asm("x6") = arg6;

	asm volatile("svc 0\n\t"
		     : "+r"(x0), "+r"(x1)
		     : "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x6)
		     : "memory");

	if (out)
		*out = x1;

	return x0;
}

#elif defined(__riscv)
/*
 * number: a0
 * arg 1: a1
 * arg 2: a2
 * arg 3: a3
 * arg 4: a4
 * arg 5: a5
 * arg 6: a6
 * return value: a0
 * additional output: a1
 */

static inline uintptr_t
syscall0(uintptr_t num, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1");

	asm volatile("ecall\n\t" : "+r"(a0) : "r"(a1) : "memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall1(uintptr_t num, uintptr_t arg1, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1;

	asm volatile("ecall\n\t" : "+r"(a0), "+r"(a1)::"memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall2(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1,
			      a2 asm("a2") = arg2;

	asm volatile("ecall\n\t" : "+r"(a0), "+r"(a1) : "r"(a2) : "memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall3(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1,
			      a2 asm("a2") = arg2, a3 asm("a3") = arg3;

	asm volatile("ecall\n\t"
		     : "+r"(a0), "+r"(a1)
		     : "r"(a2), "r"(a3)
		     : "memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall4(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1,
			      a2 asm("a2") = arg2, a3 asm("a3") = arg3,
			      a4 asm("a4") = arg4;

	asm volatile("ecall\n\t"
		     : "+r"(a0), "+r"(a1)
		     : "r"(a2), "r"(a3), "r"(a4)
		     : "memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall5(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1,
			      a2 asm("a2") = arg2, a3 asm("a3") = arg3,
			      a4 asm("a4") = arg4, a5 asm("a5") = arg5;

	asm volatile("ecall\n\t"
		     : "+r"(a0), "+r"(a1)
		     : "r"(a2), "r"(a3), "r"(a4), "r"(a5)
		     : "memory");

	if (out)
		*out = a1;

	return a0;
}

static inline uintptr_t
syscall6(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t arg6, uintptr_t *out)
{
	register uintptr_t a0 asm("a0") = num, a1 asm("a1") = arg1,
			      a2 asm("a2") = arg2, a3 asm("a3") = arg3,
			      a4 asm("a4") = arg4, a5 asm("a5") = arg5,
			      a6 asm("a6") = arg6;

	asm volatile("ecall\n\t"
		     : "+r"(a0), "+r"(a1)
		     : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6)
		     : "memory");

	if (out)
		*out = a1;

	return a0;
}

#endif

#endif /* _KEYRONEX__SYSCALL_H */
