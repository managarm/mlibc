#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <bits/feature.h>

#ifdef __MLIBC_POSIX_OPTION

#include <bits/posix/pid_t.h>
#include <bits/posix/uid_t.h>

union sigval {
	int sival_int;
	void *sival_ptr;
};

typedef struct {
	int si_signo;
	int si_code;
	int si_errno;
	pid_t si_pid;
	uid_t si_uid;
	void *si_addr;
	int si_status;
	union sigval si_value;
} siginfo_t;

#endif // __MLIBC_POSIX_OPTION

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_ERR ((__sighandler)(void *)(-1))
#define SIG_DFL ((__sighandler)(void *)(-2))
#define SIG_IGN ((__sighandler)(void *)(-3))

#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6
#define SIGPROF 7
#define SIGIO 9
#define SIGPWR 10
#define SIGRTMIN 11
#define SIGRTMAX 12

#ifdef __MLIBC_POSIX_OPTION

#define SIGALRM 8
#define SIGBUS 13
#define SIGCHLD 14
#define SIGCONT 15
#define SIGHUP 16
#define SIGKILL 17
#define SIGPIPE 18
#define SIGQUIT 19
#define SIGSTOP 20
#define SIGTSTP 21
#define SIGTTIN 22
#define SIGTTOU 23
#define SIGUSR1 24
#define SIGUSR2 25
#define SIGSYS 26
#define SIGTRAP 27
#define SIGURG 28
#define SIGVTALRM 29
#define SIGXCPU 30
#define SIGXFSZ 31
#define SIGWINCH 32
#define SIGUNUSED SIGSYS

#define SA_NOCLDSTOP (1 << 0)
#define SA_ONSTACK (1 << 1)
#define SA_RESETHAND (1 << 2)
#define SA_RESTART (1 << 3)
#define SA_SIGINFO (1 << 4)
#define SA_NOCLDWAIT (1 << 5)
#define SA_NODEFER (1 << 6)

#define MINSIGSTKSZ 2048

// constants for sigev_notify of struct sigevent
#define SIGEV_NONE 1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

// constants for sigprocmask()
#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

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

struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction)(int, siginfo_t *, void *);
};

#endif // __MLIBC_POSIX_OPTION

#ifdef __cplusplus
}
#endif

#endif // _ABIBITS_SIGNAL_H
