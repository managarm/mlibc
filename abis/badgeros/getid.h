#ifndef _ABIBITS_BADGEROS_GETID_T_H
#define _ABIBITS_BADGEROS_GETID_T_H

#include <bits/types.h>

/* ID of calling process. */
#define _GETID_PID 0
/* ID of calling process' parent. */
#define _GETID_PPID 1
/* ID of calling thread. */
#define _GETID_TID 2
/* User ID of calling process. */
#define _GETID_UID 3
/* Effective user ID of calling process. */
#define _GETID_EUID 4
/* Group ID of calling process. */
#define _GETID_GID 5
/* Effective group ID of calling process. */
#define _GETID_EGID 6

#endif /* _ABIBITS_BADGEROS_GETID_T_H */
