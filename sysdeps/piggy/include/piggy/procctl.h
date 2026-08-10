#ifndef _PIGGY_PROCCTL_H
#define _PIGGY_PROCCTL_H

#include <abi-bits/pid_t.h>
#include <stddef.h>
#include <stdint.h>

#define PROCCTL_NEXTPID 1
#define PROCCTL_PREVPID 2
#define PROCCTL_STATUS  3
#define PROCCTL_CMDLINE 4
#define PROCCTL_VMMAPS  5

#define PROCCTL_PID_END ((pid_t) -1)

#define PROCCTL_STATE_RUNNING   0
#define PROCCTL_STATE_ZOMBIE    1

struct procctl_status {
    char name[64];
    int state;
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    size_t thread_count;
};

struct procctl_vm_map {
    uint64_t start;
    uint64_t end;
    int flags;
    int prot;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int procctl(pid_t pid, int op, void* buf, size_t* len);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PIGGY_PROCCTL_H */
