#ifndef _ASM_IOCTL_H
#define _ASM_IOCTL_H

#include <sys/ioctl.h>

#define TIOCGPTN _IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK _IOW('T', 0x31, int)

#endif // _ASM_IOCTL_H
