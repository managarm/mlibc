#ifndef _ABIBITS_AUXV_H
#define _ABIBITS_AUXV_H

#define AT_HWCAP 16
#define AT_SECURE 23
#define AT_RANDOM 25
#define AT_EXECFN 31
#define AT_SYSINFO_EHDR 33

/* managarm specific auxvector entries. */

#define AT_XPIPE 0x1000
#define AT_OPENFILES 0x1001
#define AT_FS_SERVER 0x1102
#define AT_MBUS_SERVER 0x1103

#endif /* _ABIBITS_AUXV_H */
