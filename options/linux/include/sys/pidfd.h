#ifndef _SYS_PIDFD_H
#define _SYS_PIDFD_H

#include <abi-bits/fcntl.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/signal.h>

#define PIDFD_NONBLOCK O_NONBLOCK

#ifndef __MLIBC_ABI_ONLY

#ifdef __cplusplus
extern "C" {
#endif

int pidfd_open(pid_t __pid, unsigned int __flags);
pid_t pidfd_getpid(int __fd);
int pidfd_send_signal(int __pidfd, int __sig, siginfo_t *__info, unsigned int __flags);

#ifdef __cplusplus
}
#endif

#endif /* !__MLIBC_ABI_ONLY */

#endif /* _SYS_PIDFD_H */
