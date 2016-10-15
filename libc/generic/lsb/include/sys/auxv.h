
#ifndef _SYS_AUXV_H
#define _SYS_AUXV_H

#define AT_NULL 0
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_ENTRY 9

// managarm specific auxvector entries.

#define AT_XPIPE 0x1000
#define AT_OPENFILES 0x1001
#define AT_POSIX_SERVER 0x1101
#define AT_FS_SERVER 0x1102
#define AT_MBUS_SERVER 0x1103

#ifdef __cplusplus
extern "C" {
#endif

// mlibc extension: Like getauxval but handles errors in a sane way.
// Success: Return 0.
// Failure: Return -1 and set errno.
int peekauxval(unsigned long type, unsigned long *value);

unsigned long getauxval(unsigned long type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

