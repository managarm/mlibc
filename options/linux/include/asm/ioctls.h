#ifndef _ASM_IOCTLS_H
#define _ASM_IOCTLS_H

#include <linux/ioctl.h>

#define TCGETS 0x5401
#define TCSETS 0x5402
#define TIOCSCTTY 0x540E
#define TIOCGWINSZ 0x5413

#define TIOCGPTN _IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK _IOW('T', 0x31, int)
#define TIOCSIG _IOW('T', 0x36, int)

#endif // _ASM_IOCTLS_H
