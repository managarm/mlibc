
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/posix/id_t.h>
#include <abi-bits/pid_t.h>
// for siginfo_t
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// According to POSIX, <sys/wait.h> does not make rusage available.
struct rusage;

#define WCONTINUED 1
#define WNOHANG 2
#define WUNTRACED 4
#define WEXITED 8
#define WNOWAIT 16
#define WSTOPPED 32

#define WCOREFLAG 0x80

// TODO: #error if int is smaller than 32 bits

#define WEXITSTATUS(x) ((x) & 0x000000FF)
#define WIFCONTINUED(x) ((x) & 0x00000100)
#define WIFEXITED(x) ((x) & 0x00000200)
#define WIFSIGNALED(x) ((x) & 0x00000400)
#define WIFSTOPPED(x) ((x) & 0x00000800)
#define WSTOPSIG(x) (((x) & 0x00FF0000) >> 16)
#define WTERMSIG(x) (((x) & 0xFF000000) >> 24)
#define WCOREDUMP(x) ((x) & WCOREFLAG)

// TODO: move to own file and include in sys/types.h
typedef enum {
	P_ALL, P_PID, P_PGID
} idtype_t;

pid_t wait(int *status);
int waitid(idtype_t idtype, id_t id, siginfo_t *siginfo, int flags);
pid_t waitpid(pid_t pid, int *status, int flags);

// GNU extensions.
pid_t wait3(int *, int, struct rusage *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_WAIT_H

