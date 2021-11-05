
#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include <sys/ioctl.h>

#define BLKROGET _IO(0x12, 94)
#define BLKRRPART _IO(0x12, 95)
#define BLKIOMIN _IO(0x12, 120)
#define BLKIOOPT _IO(0x12, 121)
#define BLKALIGNOFF _IO(0x12, 122)
#define BLKPBSZGET _IO(0x12, 123)

#define FS_NOCOW_FL		0x00800000

#endif // _LINUX_FS_H

