
#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <abi-bits/pid_t.h>
#include <abi-bits/termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/winsize.h>

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include <sys/ttydefaults.h>
#endif

/* baud rate constants for speed_t */
#define B0       0
#define B50      1
#define B75      2
#define B110     3
#define B134     4
#define B150     5
#define B200     6
#define B300     7
#define B600     8
#define B1200    9
#define B1800    10
#define B2400    11
#define B4800    12
#define B9600    13
#define B19200   14
#define B38400   15
#define B57600   0010001
#define B115200  0010002
#define B230400  0010003
#define B460800  0010004
#define B500000  0010005
#define B576000  0010006
#define B921600  0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017

/* constants for tcsetattr() */
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/* constants for tcflush() */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/* constants for tcflow() */
#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

#define TIOCM_DTR 0x002
#define TIOCM_RTS 0x004
#define TIOCM_CTS 0x020
#define TIOCM_CAR 0x040
#define TIOCM_RNG 0x080
#define TIOCM_DSR 0x100
#define TIOCM_RI TIOCM_RNG
#define TIOCM_CD TIOCM_CAR

#ifndef __MLIBC_ABI_ONLY

speed_t cfgetispeed(const struct termios *__tios);
speed_t cfgetospeed(const struct termios *__tios);
int cfsetispeed(struct termios *__tios, speed_t __speed);
int cfsetospeed(struct termios *__tios, speed_t __speed);
void cfmakeraw(struct termios *__tios);
int tcdrain(int __fd);
int tcflow(int __fd, int __action);
int tcflush(int __fd, int __queue_selector);
int tcgetattr(int fd, struct termios *__attr);
pid_t tcgetsid(int __fd);
int tcsendbreak(int __fd, int __duration);
int tcsetattr(int __fd, int __optional_actions, const struct termios *__attr);

#endif /* !__MLIBC_ABI_ONLY */

/* This is a linux extension */

#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCGSID 0x5429

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_H */

