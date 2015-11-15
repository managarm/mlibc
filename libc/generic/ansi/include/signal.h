
#ifndef _SIGNAL_H
#define _SIGNAL_H

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

// [7.14.1] signal() function

__sighandler signal(int sig, __sighandler handler);

// [7.14.2] raise() function

int raise(int sig);

#ifdef __cplusplus
}
#endif

#endif // _SIGNAL_H

