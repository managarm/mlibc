
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

typedef struct __stack {
        void *ss_sp;
        size_t ss_size;
        int ss_flags;
} stack_t;

typedef struct __ucontext {
        unsigned long uc_flags;
        struct __ucontext *uc_link;
        stack_t uc_stack;
        mcontext_t uc_mcontext;
        sigset_t uc_sigmask;
} ucontext_t;

// functions to block / wait for signals
int sigsuspend(const sigset_t *);
int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict);

int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict);
int pthread_kill(pthread_t, int);

// functions to handle signals
int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
int sigpending(sigset_t *);

int siginterrupt(int sig, int flag);

// functions to raise signals
int kill(pid_t, int);
int killpg(int, int);

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout);

int sigwait(const sigset_t *set, int *sig);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_SIGNAL_H

