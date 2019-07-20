#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <bits/feature.h>

// On Linux, sys/ioctl.h includes the termios ioctls.
#if __MLIBC_LINUX_OPTION
#	include <asm/ioctls.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int fd, unsigned long request, ...);

#ifdef __cplusplus
}
#endif

#endif // _SYS_IOCTL_H
