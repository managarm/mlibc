#ifndef _SYS_MAC_H
#define _SYS_MAC_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_CAP_SCHED     (1 << 0)
#define MAC_CAP_SPAWN     (1 << 1)
#define MAC_CAP_ENTROPY   (1 << 2)
#define MAC_CAP_SYS_MEM   (1 << 3)
#define MAC_CAP_USE_NET   (1 << 4)
#define MAC_CAP_SYS_NET   (1 << 5)
#define MAC_CAP_SYS_MNT   (1 << 6)
#define MAC_CAP_SYS_PWR   (1 << 7)
#define MAC_CAP_PTRACE    (1 << 8)
#define MAC_CAP_SETUID    (1 << 9)
#define MAC_CAP_SYS_MAC   (1 << 10)
#define MAC_CAP_CLOCK     (1 << 11)
#define MAC_CAP_SIGNALALL (1 << 12)
#define MAC_CAP_SETGID    (1 << 13)
#define MAC_CAP_IPC       (1 << 14)
#define MAC_CAP_SYS_LOG   (1 << 15)

unsigned long get_mac_capabilities(void);
int set_mac_capabilities(unsigned long request);

#define MAC_PERM_CONTENTS (1 << 0)
#define MAC_PERM_READ     (1 << 1)
#define MAC_PERM_WRITE    (1 << 2)
#define MAC_PERM_EXEC     (1 << 3)
#define MAC_PERM_APPEND   (1 << 4)
#define MAC_PERM_FLOCK    (1 << 5)
#define MAC_PERM_DEV      (1 << 6)
int add_mac_permissions(const char *path, int flags);

#define MAC_DENY            (1 << 0)
#define MAC_DENY_AND_SCREAM (1 << 1)
#define MAC_KILL            (1 << 2)
int set_mac_enforcement(unsigned long enforcement);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MAC_H */
