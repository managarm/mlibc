#ifndef _MLIBC_WRAPPERS_H
#define _MLIBC_WRAPPERS_H

#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo);
int rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_WRAPPERS_H */
