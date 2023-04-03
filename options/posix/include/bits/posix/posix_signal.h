
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

// Start Glibc stuff

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
// End Glibc stuff

typedef unsigned long int greg_t;

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

// functions to block / wait for signals
int sigsuspend(const sigset_t *);
int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict);

int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict);
int pthread_kill(pthread_t, int);

// functions to handle signals
int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
int sigpending(sigset_t *);

int siginterrupt(int sig, int flag);

int sigaltstack(const stack_t *__restrict ss, stack_t *__restrict oss);

// functions to raise signals
int kill(pid_t, int);
int killpg(int, int);

int sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout);
int sigwait(const sigset_t *__restrict set, int *__restrict sig);
int sigwaitinfo(const sigset_t *__restrict set, siginfo_t *__restrict info);

// Glibc extension
#if __MLIBC_GLIBC_OPTION
int sigisemptyset(const sigset_t *set);
#endif // __MLIBC_GLIBC_OPTION

int sigqueue(pid_t pid, int sig, const union sigval value);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_SIGNAL_H

