#ifndef _NET_IF_PPP_H
#define _NET_IF_PPP_H

#include <mlibc-config.h>

#if __MLIBC_LINUX_OPTION
#include <asm/ioctl.h>
#include <linux/ppp_defs.h>

#define PPPIOCGFLAGS _IOR('t', 90, int)
#define PPPIOCSFLAGS _IOW('t', 89, int)
#define PPPIOCGASYNCMAP _IOR('t', 88, int)
#define PPPIOCSASYNCMAP _IOW('t', 87, int)
#define PPPIOCGUNIT _IOR('t', 86, int)
#define PPPIOCSMRU _IOW('t', 82, int)
#define PPPIOCSMAXCID _IOW('t', 81, int)
#define PPPIOCGXASYNCMAP _IOR('t', 80, ext_accm)
#define PPPIOCSXASYNCMAP _IOW('t', 79, ext_accm)
#define PPPIOCGDEBUG _IOR('t', 65, int)
#define PPPIOCSDEBUG _IOW('t', 64, int)
#endif

#endif /* _NET_IF_PPP_H */
