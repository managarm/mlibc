#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <bits/feature.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/uid_t.h>

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

// TODO: replace this by uint64_t
typedef long sigset_t;

#ifdef __MLIBC_POSIX_OPTION

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

#define MINSIGSTKSZ 2048

// constants for sigev_notify of struct sigevent
#define SIGEV_NONE 1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

// constants for sigprocmask()
#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

#define SI_USER 0

#define NSIG 65

struct sigevent {
	int sigev_notify;
	int sigev_signo;
	union sigval sigev_value;
	void (*sigev_notify_function)(union sigval);
	// MISSING: sigev_notify_attributes
};

#endif // __MLIBC_POSIX_OPTION

struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction)(int, siginfo_t *, void *);
};

#ifdef __cplusplus
}
#endif

#endif // _ABIBITS_SIGNAL_H
