#ifndef _ASM_IOCTL_H
#define _ASM_IOCTL_H

#include <sys/ioctl.h>

#define TCGETS 0x5401
#define TCSETS 0x5402

#define TIOCGPTN _IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK _IOW('T', 0x31, int)

#endif // _ASM_IOCTL_H
