#ifndef _SYS_MAC_H
#define _SYS_MAC_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_CAP_SCHED   0b00000000001
#define MAC_CAP_SPAWN   0b00000000010
#define MAC_CAP_ENTROPY 0b00000000100
#define MAC_CAP_SYS_MEM 0b00000001000
#define MAC_CAP_USE_NET 0b00000010000
#define MAC_CAP_SYS_NET 0b00000100000
#define MAC_CAP_SYS_MNT 0b00001000000
#define MAC_CAP_SYS_PWR 0b00010000000
#define MAC_CAP_PTRACE  0b00100000000
#define MAC_CAP_SETUID  0b01000000000
#define MAC_CAP_SYS_MAC 0b10000000000
#define MAC_CAP_CLOCK  0b100000000000
#define MAC_CAP_SIGNALALL 0b1000000000000
unsigned long get_mac_capabilities(void);
int set_mac_capabilities(unsigned long request);

#define MAC_PERM_CONTENTS 0b0000001
#define MAC_PERM_READ     0b0000010
#define MAC_PERM_WRITE    0b0000100
#define MAC_PERM_EXEC     0b0001000
#define MAC_PERM_APPEND   0b0010000
#define MAC_PERM_FLOCK    0b0100000
#define MAC_PERM_DEV      0b1000000
int add_mac_permissions(const char *path, int flags);

#define MAC_DENY            0b001
#define MAC_DENY_AND_SCREAM 0b010
#define MAC_KILL            0b100
int set_mac_enforcement(unsigned long enforcement);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MAC_H
