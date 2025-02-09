#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <polaris/syscall.h>
#include <sys/ioctl.h>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	int ret;
	int fds[2];

    ret = syscall(0xff, fds);
	
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
	
	*mfd = fds[0];
	*sfd = fds[1];

	if (name != NULL) {
		strcpy(name, "tty0");
#if 0
		ret = ttyname_r(*mfd, name, (size_t)-1);
		if (ret) {
			return -1;
		}
#endif
	}
	if (ios != NULL) {
		ret = ioctl(*mfd, TCSETS, ios);
		if (ret) {
			return -1;
		}
	}
	if (win != NULL) {
		ret = ioctl(*mfd, TIOCSWINSZ, win);
		if (ret) {
			return -1;
		}
	}
	return ret;
}
