
#ifndef  _PTY_H
#define  _PTY_H

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int openpty(int *, int *, char *, const struct termios *, const struct winsize *);
int forkpty(int *, char *, const struct termios *, const struct winsize *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _PTY_H

