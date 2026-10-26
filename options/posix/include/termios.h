
#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <mlibc-config.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/termios.h>

#include <bits/winsize.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include <sys/ttydefaults.h>
#endif

#ifndef __MLIBC_ABI_ONLY

speed_t cfgetispeed(const struct termios *__tios);
speed_t cfgetospeed(const struct termios *__tios);
int cfsetispeed(struct termios *__tios, speed_t __speed);
int cfsetospeed(struct termios *__tios, speed_t __speed);
int tcdrain(int __fd);
int tcflow(int __fd, int __action);
int tcflush(int __fd, int __queue_selector);
int tcgetattr(int __fd, struct termios *__attr);
pid_t tcgetsid(int __fd);
int tcsendbreak(int __fd, int __duration);
int tcsetattr(int __fd, int __optional_actions, const struct termios *__attr);
int tcgetwinsize(int __fd, struct winsize *__winsz);
int tcsetwinsize(int __fd, const struct winsize *__winsz);

#if defined(_DEFAULT_SOURCE)
void cfmakeraw(struct termios *__tios);
#endif /* defined(_DEFAULT_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#if defined(_DEFAULT_SOURCE)
#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCGSID 0x5429
#endif /* defined(_DEFAULT_SOURCE) */

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_H */

