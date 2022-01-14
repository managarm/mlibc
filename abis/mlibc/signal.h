#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <abi-bits/pid_t.h>
#include <abi-bits/uid_t.h>
#include <bits/size_t.h>

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

#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL SIGIO
#define SIGPWR 30
#define SIGSYS 31
#define SIGRTMIN 32
#define SIGRTMAX 33
#define SIGCANCEL 34

// TODO: replace this by uint64_t
typedef long sigset_t;

#define SIGUNUSED SIGSYS

// constants for sigprocmask()
#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

#define SA_NOCLDSTOP (1 << 0)
#define SA_ONSTACK (1 << 1)
#define SA_RESETHAND (1 << 2)
#define SA_RESTART (1 << 3)
#define SA_SIGINFO (1 << 4)
#define SA_NOCLDWAIT (1 << 5)
#define SA_NODEFER (1 << 6)

#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192
#define SS_ONSTACK 1
#define SS_DISABLE 2

typedef struct __stack {
        void *ss_sp;
        size_t ss_size;
        int ss_flags;
} stack_t;

// constants for sigev_notify of struct sigevent
#define SIGEV_NONE 1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

#define SI_ASYNCNL (-60)
#define SI_TKILL (-6)
#define SI_SIGIO (-5)
#define SI_ASYNCIO (-4)
#define SI_MESGQ (-3)
#define SI_TIMER (-2)
#define SI_QUEUE (-1)
#define SI_USER 0
#define SI_KERNEL 128

#define NSIG 65

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

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

// TODO: this struct won't work on all arches (for example aarch64) but
// we don't have an arch specific abi folder for mlibc yet.
typedef struct {
	unsigned long oldmask;
	unsigned long gregs[16];
	unsigned long pc, pr, sr;
	unsigned long gbr, mach, macl;
	unsigned long fpregs[16];
	unsigned long xfpregs[16];
	unsigned int fpscr, fpul, ownedfp;
} mcontext_t;

#ifdef __cplusplus
}
#endif

#endif // _ABIBITS_SIGNAL_H
