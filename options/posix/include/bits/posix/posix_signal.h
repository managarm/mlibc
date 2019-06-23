
#ifndef MLIBC_POSIX_SIGNAL_H
#define MLIBC_POSIX_SIGNAL_H

#include <bits/posix/siginfo_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIGALRM 8
#define SIGBUS 9
#define SIGCHLD 10
#define SIGCONT 11
#define SIGHUP 12
#define SIGKILL 13
#define SIGPIPE 14
#define SIGQUIT 15
#define SIGSTOP 16
#define SIGTSTP 17
#define SIGTTIN 18
#define SIGTTOU 19
#define SIGUSR1 20
#define SIGUSR2 21
#define SIGSYS 22
#define SIGTRAP 23
#define SIGURG 24
#define SIGVTALRM 25
#define SIGXCPU 26
#define SIGXFSZ 27
#define SIGWINCH 28
#define SIGUNUSED SIGSYS

#define SA_NOCLDSTOP (1 << 0)
#define SA_ONSTACK (1 << 1)
#define SA_RESETHAND (1 << 2)
#define SA_RESTART (1 << 3)
#define SA_SIGINFO (1 << 4)
#define SA_NOCLDWAIT (1 << 5)
#define SA_NODEFER (1 << 6)

#define NSIG 65

// TODO: replace this by uint64_t
typedef long sigset_t;

struct sigevent {
	int sigev_notify;
	int sigev_signo;
	union sigval sigev_value;
	void (*sigev_notify_function)(union sigval);
	// MISSING: sigev_notify_attributes
};

// constants for sigev_notify of struct sigevent
#define SIGEV_NONE 1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction)(int, siginfo_t *, void *);
};

// constants for sigprocmask()
#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

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

// functions to handle signals
int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);

// functions to raise signals
int kill(pid_t pid, int sig);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_SIGNAL_H

