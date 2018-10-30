#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <bits/feature.h>

#ifdef __cplusplus
extern "C" {
#endif

// [7.14] Signal handling basics

typedef int sig_atomic_t;

// Argument for signal()
typedef void (*__sighandler) (int);

void __signalDfl(int signal);
void __signalErr(int signal);
void __signalIgn(int signal);

#define SIG_DFL __signalDfl
#define SIG_ERR __signalErr
#define SIG_IGN __signalIgn

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

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

// [7.14.1] signal() function

__sighandler signal(int sig, __sighandler handler);

// [7.14.2] raise() function

int raise(int sig);

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_signal.h>
#endif

#endif // _SIGNAL_H
