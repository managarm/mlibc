#ifndef _ABIBITS_AUXV_H
#define _ABIBITS_AUXV_H

#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_ENTRY 9
#define AT_RANDOM 25
#define AT_EXECFN 31

// Sigma specific auxvector entries.

#define AT_VFS_SERVER 0x1000
#define AT_KBUS_SERVER 0x1001

#endif
