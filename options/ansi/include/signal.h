#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <abi-bits/signal.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

// [7.14] Signal handling basics

typedef int sig_atomic_t;

// Argument for signal()
typedef void (*__sighandler) (int);

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

#ifdef __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_signal.h>
#endif

#endif // _SIGNAL_H
