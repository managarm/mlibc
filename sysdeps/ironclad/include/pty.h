#ifndef _PTY_H
#define _PTY_H

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win);

#ifdef __cplusplus
}
#endif

#endif /* _PTY_H */
