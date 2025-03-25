#ifndef _SYS_PIDFD_H
#define _SYS_PIDFD_H

#include <abi-bits/fcntl.h>
#include <abi-bits/pid_t.h>

#define PIDFD_NONBLOCK O_NONBLOCK

#ifndef __MLIBC_ABI_ONLY

#ifdef __cplusplus
extern "C" {
#endif

int pidfd_open(pid_t __pid, unsigned int __flags);

#ifdef __cplusplus
}
#endif

#endif /* !__MLIBC_ABI_ONLY */

#endif /* _SYS_PIDFD_H */
