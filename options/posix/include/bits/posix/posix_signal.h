
#ifndef MLIBC_POSIX_SIGNAL_H
#define MLIBC_POSIX_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/posix/pthread_t.h>
#include <bits/sigset_t.h>
#include <stddef.h>
#include <stdint.h>
#include <mlibc-config.h>

#define FPE_INTDIV      1       /* integer divide by zero */
#define FPE_INTOVF      2       /* integer overflow */
#define FPE_FLTDIV      3       /* floating point divide by zero */
#define FPE_FLTOVF      4       /* floating point overflow */
#define FPE_FLTUND      5       /* floating point underflow */
#define FPE_FLTRES      6       /* floating point inexact result */
#define FPE_FLTINV      7       /* floating point invalid operation */
#define FPE_FLTSUB      8       /* subscript out of range */

#define TRAP_BRKPT      1       /* process breakpoint */
#define TRAP_TRACE      2       /* process trace trap */

#if defined(__x86_64__)
/* Start Glibc stuff */

struct _libc_fpxreg {
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int __glibc_reserved1[3];
};

struct _libc_xmmreg {
  uint32_t element[4];
};

struct _libc_fpstate {
  uint16_t cwd;
  int16_t  swd;
  uint16_t ftw;
  uint16_t fop;
  uint64_t rip;
  uint64_t dp;
  uint32_t mxcsr;
  uint32_t mxcr_mask;
  struct _libc_fpxreg _st[8];
  struct _libc_xmmreg _xmm[16];
  uint32_t __glibc_reserved1[24];
};

typedef struct _libc_fpstate *fpregset_t;
/* End Glibc stuff */

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

#define TRAP_BRKPT      1       /* process breakpoint */
#define TRAP_TRACE      2       /* process trace trap */

#ifndef __MLIBC_ABI_ONLY

/* functions to block / wait for signals */
int sigsuspend(const sigset_t *__sigmask);
int sigprocmask(int __how, const sigset_t *__restrict __sigmask, sigset_t *__restrict __oldmask);

int pthread_sigmask(int __how, const sigset_t *__restrict __sigmask, sigset_t *__restrict __oldmask);
int pthread_kill(pthread_t __thrd, int __sig);

/* functions to handle signals */
int sigaction(int __signum, const struct sigaction *__restrict __act, struct sigaction *__restrict __oldact);
int sigpending(sigset_t *__set);

int siginterrupt(int __sig, int __flag);

int sigaltstack(const stack_t *__restrict __ss, stack_t *__restrict __oss);

/* functions to raise signals */
int kill(pid_t __pid, int __number);
int killpg(int __pgrp, int __sig);

int sigtimedwait(const sigset_t *__restrict __set, siginfo_t *__restrict __info, const struct timespec *__restrict __timeout);
int sigwait(const sigset_t *__restrict __set, int *__restrict __sig);
int sigwaitinfo(const sigset_t *__restrict __set, siginfo_t *__restrict __info);

/* Glibc extension */
#if __MLIBC_GLIBC_OPTION
int sigisemptyset(const sigset_t *__set);
#endif /* __MLIBC_GLIBC_OPTION */

int sigqueue(pid_t __pid, int __sig, const union sigval __value);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_SIGNAL_H */

