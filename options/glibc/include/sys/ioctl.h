#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <mlibc-config.h>

// On Linux, sys/ioctl.h includes the termios ioctls.
#ifdef __MLIBC_LINUX_OPTION
#	include <asm/ioctls.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int fd, unsigned long request, ...);

#define FIONREAD 0x541B
#define FIONBIO 0x5421
#define FIONCLEX 0x5450
#define FIOCLEX 0x5451

#define SIOCGIFNAME 0x8910
#define SIOCGIFCONF 0x8912
#define SIOCGIFFLAGS 0x8913
#define SIOCSIFFLAGS 0x8914
#define SIOCGIFINDEX 0x8933

#define SIOCPROTOPRIVATE 0x89E0
#define SIOCDEVPRIVATE 0x89F0

#ifdef __cplusplus
}
#endif

#endif // _SYS_IOCTL_H
