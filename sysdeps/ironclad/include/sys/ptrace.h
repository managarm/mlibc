#ifndef _SYS_PTRACE_H
#define _SYS_PTRACE_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PTRACE_ATTACH  1
#define PTRACE_DETACH  2
#define PTRACE_CONT    3
#define PTRACE_SYSCALL 4
#define PTRACE_GETREGS 5
int ptrace(int request, pid_t pid, void *addr, void *data);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PTRACE_H */
