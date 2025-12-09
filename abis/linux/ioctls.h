#ifndef _ABIBITS_IOCTLS_H
#define _ABIBITS_IOCTLS_H

#include <mlibc-config.h>

#define TIOCEXCL 0x540C
#define TIOCNXCL 0x540D
#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCMGET 0x5415
#define TIOCMBIS 0x5416
#define TIOCMBIC 0x5417
#define TIOCGSID 0x5429

#if __MLIBC_LINUX_OPTION
#include <asm/ioctls.h>
#include <linux/sockios.h>
#endif /* __MLIBC_LINUX_OPTION */

#endif /* _ABIBITS_IOCTLS_H */
