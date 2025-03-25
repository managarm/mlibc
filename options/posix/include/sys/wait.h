
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/posix/id_t.h>
#include <abi-bits/pid_t.h>
/* for siginfo_t */
#include <abi-bits/signal.h>
#include <abi-bits/wait.h>

#ifdef __cplusplus
extern "C" {
#endif

/* According to POSIX, <sys/wait.h> does not make rusage available. */
struct rusage;

/* TODO: move to own file and include in sys/types.h */
typedef enum {
	P_ALL, P_PID, P_PGID, P_PIDFD
} idtype_t;

#ifndef __MLIBC_ABI_ONLY

pid_t wait(int *__status);
int waitid(idtype_t __idtype, id_t __id, siginfo_t *__siginfo, int __flags);
pid_t waitpid(pid_t __pid, int *__status, int __flags);

/* GNU extensions. */
pid_t wait3(int *__status, int __options, struct rusage *__ru);
pid_t wait4(pid_t __pid, int *__status, int __options, struct rusage *__ru);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_WAIT_H */

