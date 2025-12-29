
#ifndef _MLIBC_POSIX_SIGNAL_H
#define _MLIBC_POSIX_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/posix/pthread_attr_t.h>
#include <bits/posix/pthread_t.h>
#include <bits/sigset_t.h>
#include <bits/types.h>

#if defined(__x86_64__)

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
#define TRAP_BRKPT      1       /* process breakpoint */
#define TRAP_TRACE      2       /* process trace trap */
#endif

#if defined(_DEFAULT_SOURCE)
struct _libc_fpxreg {
	unsigned short int significand[4];
	unsigned short int exponent;
	unsigned short int __glibc_reserved1[3];
};

struct _libc_xmmreg {
	__mlibc_uint32 element[4];
};

struct _libc_fpstate {
	__mlibc_uint16 cwd;
	__mlibc_int16  swd;
	__mlibc_uint16 ftw;
	__mlibc_uint16 fop;
	__mlibc_uint64 rip;
	__mlibc_uint64 dp;
	__mlibc_uint32 mxcsr;
	__mlibc_uint32 mxcr_mask;
	struct _libc_fpxreg _st[8];
	struct _libc_xmmreg _xmm[16];
	__mlibc_uint32 __glibc_reserved1[24];
};

typedef struct _libc_fpstate *fpregset_t;
#endif

typedef unsigned long int greg_t;
#endif

#define FPE_INTDIV      1       /* integer divide by zero */
#define FPE_INTOVF      2       /* integer overflow */
#define FPE_FLTDIV      3       /* floating point divide by zero */
#define FPE_FLTOVF      4       /* floating point overflow */
#define FPE_FLTUND      5       /* floating point underflow */
#define FPE_FLTRES      6       /* floating point inexact result */
#define FPE_FLTINV      7       /* floating point invalid operation */
#define FPE_FLTSUB      8       /* subscript out of range */

#define SIG2STR_MAX 16

#ifndef __MLIBC_ABI_ONLY

/* functions to block / wait for signals */
int sigsuspend(const sigset_t *__sigmask);
int sigprocmask(int __how, const sigset_t *__restrict __sigmask, sigset_t *__restrict __oldmask);

int pthread_sigmask(int __how, const sigset_t *__restrict __sigmask, sigset_t *__restrict __oldmask);
int pthread_kill(pthread_t __thrd, int __sig);

/* functions to handle signals */
int sigaction(int __signum, const struct sigaction *__restrict __act, struct sigaction *__restrict __oldact);
int sigpending(sigset_t *__set);

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
/* removed in POSIX issue 8 */
int siginterrupt(int __sig, int __flag);
#endif /* defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024) */

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
int sigaltstack(const stack_t *__restrict __ss, stack_t *__restrict __oss);

int killpg(int __pgrp, int __sig);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN */

int kill(pid_t __pid, int __number);

int sigtimedwait(const sigset_t *__restrict __set, siginfo_t *__restrict __info, const struct timespec *__restrict __timeout);
int sigwait(const sigset_t *__restrict __set, int *__restrict __sig);
int sigwaitinfo(const sigset_t *__restrict __set, siginfo_t *__restrict __info);

#if __MLIBC_GLIBC_OPTION && defined(_GNU_SOURCE)
int sigisemptyset(const sigset_t *__set);
#endif /* __MLIBC_GLIBC_OPTION && defined(_GNU_SOURCE) */

int sigqueue(pid_t __pid, int __sig, const union sigval __value);

int sig2str(int __signum, char *__str);
int str2sig(const char *__restrict __str, int *__restrict __pnum);

void psiginfo(const siginfo_t *__pinfo, const char *__message);
void psignal(int __signum, const char *__message);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_POSIX_SIGNAL_H */

