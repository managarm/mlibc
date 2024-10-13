#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <abi-bits/signal.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* [7.14] Signal handling basics */

typedef int sig_atomic_t;

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

#ifndef __MLIBC_ABI_ONLY

/* [7.14.1] signal() function */

__sighandler signal(int __sig, __sighandler __handler);

/* [7.14.2] raise() function */

int raise(int __sig);

#endif /* !__MLIBC_ABI_ONLY */

#define _NSIG NSIG

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_signal.h>
#endif

#if __MLIBC_GLIBC_OPTION
#	include <bits/glibc/glibc_signal.h>
#endif

#endif /* _SIGNAL_H */
