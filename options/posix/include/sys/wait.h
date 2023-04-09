
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/posix/id_t.h>
#include <abi-bits/pid_t.h>
// for siginfo_t
#include <abi-bits/signal.h>
#include <abi-bits/wait.h>

#ifdef __cplusplus
extern "C" {
#endif

// According to POSIX, <sys/wait.h> does not make rusage available.
struct rusage;

// TODO: move to own file and include in sys/types.h
typedef enum {
	P_ALL, P_PID, P_PGID
} idtype_t;

#ifndef __MLIBC_ABI_ONLY

pid_t wait(int *status);
int waitid(idtype_t idtype, id_t id, siginfo_t *siginfo, int flags);
pid_t waitpid(pid_t pid, int *status, int flags);

// GNU extensions.
pid_t wait3(int *, int, struct rusage *);
pid_t wait4(pid_t pid, int *status, int options, struct rusage *ru);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_WAIT_H

