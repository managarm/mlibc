
#ifndef MLIBC_POSIX_SIGNAL_H
#define MLIBC_POSIX_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/posix/pthread_t.h>

// functions to manage sigset_t
int sigemptyset(sigset_t *);
int sigfillset(sigset_t *);
int sigaddset(sigset_t *, int);
int sigdelset(sigset_t *, int);
int sigismember(const sigset_t *set, int signo);

// functions to block / wait for signals
int sigsuspend(const sigset_t *);
int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict);

int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict);
int pthread_kill(pthread_t, int);

// functions to handle signals
int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
int sigpending(sigset_t *);

// functions to raise signals
int kill(pid_t, int);
int killpg(int, int);

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout);

int sigwait(const sigset_t *set, int *sig);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_SIGNAL_H

