
#ifndef  _PTY_H
#define  _PTY_H

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int openpty(int *__mfd, int *__sfd, char *__name, const struct termios *__ios, const struct winsize *__win);
int forkpty(int *__mfd, char *__name, const struct termios *__ios, const struct winsize *__win);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PTY_H */

