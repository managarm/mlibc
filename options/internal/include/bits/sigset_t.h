#ifndef _MLIBC_INTERNAL_BITS_SIGSET_T_H
#define _MLIBC_INTERNAL_BITS_SIGSET_T_H

#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* functions to manage sigset_t */
int sigemptyset(sigset_t *__sigset);
int sigfillset(sigset_t *__sigset);
int sigaddset(sigset_t *__sigset, int __sig);
int sigdelset(sigset_t *__sigset, int __sig);
int sigismember(const sigset_t *__sigset, int __sig);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_INTERNAL_BITS_SIGSET_T_H */
