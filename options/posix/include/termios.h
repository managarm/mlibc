
#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <mlibc-config.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include <sys/ttydefaults.h>
#endif

/* constants for tcflush() */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/* constants for tcflow() */
#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

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

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_H */

