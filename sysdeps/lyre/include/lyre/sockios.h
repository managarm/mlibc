#ifndef _LYRE__SOCKIOS_H
#define _LYRE__SOCKIOS_H

#include <asm/ioctls.h>

#define SIOCINQ FIONREAD
#define SIOCOUTQ TIOCOUTQ

#define SIOCGIFNAME	0x8910 /* get interface name */
#define SIOCGIFFLAGS 0x8911 /* get flags */
#define SIOCSIFFLAGS 0x8912 /* set flags */
#define SIOCGIFADDR	0x8913 /* get interface ip */
#define SIOCSIFADDR	0x8914 /* set interface ip */
#define SIOCGIFNETMASK 0x8915 /* set netmask */
#define SIOCSIFNETMASK 0x8916 /* get netmask */
#define SIOCGIFMTU 0x8917 /* get mtu */
#define SIOCSIFMTU 0x8918 /* set mtu */
#define SIOCSIFNAME	0x8919 /* set interface name */
#define	SIOCSIFHWADDR 0x891a /* set mac address */
#define SIOCGIFHWADDR 0x891b /* get mac address */
#define SIOCGIFINDEX 0x891c /* get interface index */
#define SIOCGIFGATEWAY 0x891d /* get gateway ip */
#define SIOCSIFGATEWAY 0x891e /* set gateway ip */

#endif
