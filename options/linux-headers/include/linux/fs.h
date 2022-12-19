
#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include <sys/ioctl.h>

#define BLKROGET _IO(0x12, 94)
#define BLKRRPART _IO(0x12, 95)
#define BLKIOMIN _IO(0x12, 120)
#define BLKIOOPT _IO(0x12, 121)
#define BLKALIGNOFF _IO(0x12, 122)
#define BLKPBSZGET _IO(0x12, 123)

#define FS_IOC_GETFLAGS _IOR('f', 1, long)
#define FS_IOC_SETFLAGS _IOW('f', 2, long)

#define FS_COMPR_FL		0x00000004
#define FS_IMMUTABLE_FL	0x00000010
#define FS_NOATIME_FL	0x00000080
#define FS_NOCOMP_FL	0x00000400
#define FS_NOCOW_FL		0x00800000

#endif // _LINUX_FS_H

