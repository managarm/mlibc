#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <abi-bits/pid_t.h>
#include <abi-bits/sigevent.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/uid_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*__sighandler)(int);

#define SIG_ERR ((__sighandler)(void*) (-1))
#define SIG_DFL ((__sighandler)(void*) (0))
#define SIG_IGN ((__sighandler)(void*) (1))

#define SIGABRT     1
#define SIGALRM     2
#define SIGBUS      3
#define SIGCHLD     4
#define SIGCONT     5
#define SIGFPE      6
#define SIGHUP      7
#define SIGILL      8
#define SIGINT      9
#define SIGKILL     10
#define SIGPIPE     11
#define SIGQUIT     12
#define SIGSEGV     13
#define SIGSTOP     14
#define SIGTERM     15
#define SIGTSTP     16
#define SIGTTIN     17
#define SIGTTOU     18
#define SIGUSR1     19
#define SIGUSR2     20
#define SIGWINCH    21
#define SIGSYS      22
#define SIGTRAP     23
#define SIGURG      24
#define SIGVTALRM   25
#define SIGXCPU     26
#define SIGXFSZ     27

#define SIGCANCEL   28
#define SIGIO       29
#define SIGPOLL     30
#define SIGPROF     31
#define SIGPWR      32

#define SIGRTMIN    33
#define SIGRTMAX    64

#define NSIG 64 + 1
#define _NSIG NSIG

#define SI_ASYNCNL  (-60)
#define SI_TKILL    (-6)
#define SI_SIGIO    (-5)
#define SI_ASYNCIO  (-4)
#define SI_MESGQ    (-3)
#define SI_TIMER    (-2)
#define SI_QUEUE    (-1)
#define SI_USER     0
#define SI_KERNEL   128

typedef struct {
    int si_signo;
    int si_code;
    int si_errno;
    pid_t si_pid;
    void *si_addr;
    int si_status;
    union sigval si_value;
} siginfo_t;

#define SA_NODEFER      (1 << 0)
#define SA_ONSTACK      (1 << 1)
#define SA_RESETHAND    (1 << 2)
#define SA_RESTART      (1 << 3)
#define SA_SIGINFO      (1 << 4)

struct sigaction {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t*, void*);
    };
    int sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask;
};

#define SS_DISABLE (1 << 0)
#define SS_ONSTACK (1 << 1)

typedef struct {
    void *ss_sp;
    size_t ss_size;
    int ss_flags;
} stack_t;

#define SIG_BLOCK       1
#define SIG_UNBLOCK     2
#define SIG_SETMASK     3

#define MINSIGSTKSZ     2048
#define SIGSTKSZ        8192

#define SIGEV_NONE      1
#define SIGEV_SIGNAL    2
#define SIGEV_THREAD    3

typedef struct {
    unsigned long gregs[16];
} mcontext_t;

typedef struct __ucontext {
    struct __ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    sigset_t uc_sigmask;
} ucontext_t;

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGNAL_H */
