#ifndef _ABIBITS_UTMP_DEFINES_H
#define _ABIBITS_UTMP_DEFINES_H

#include <mlibc-config.h>

#define EMPTY 0
#define RUN_LVL 1
#define BOOT_TIME 2
#define NEW_TIME 3
#define OLD_TIME 4
#define INIT_PROCESS 5
#define LOGIN_PROCESS 6
#define USER_PROCESS 7
#define DEAD_PROCESS 8

#ifdef _GNU_SOURCE
#define ACCOUNTING 9
#endif

#if __MLIBC_LINUX_OPTION
#define UTMP_FILE "/var/run/utmp"
#define WTMP_FILE "/var/log/wtmp"
#endif /* __MLIBC_LINUX_OPTION */

#endif /* _ABIBITS_UTMP_DEFINES_H */
