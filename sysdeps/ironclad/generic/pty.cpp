#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	int ret;
	int fds[2];
	SYSCALL1(SYSCALL_OPENPTY, fds);
	if (errno) {
		return -1;
	}
	*mfd = fds[0];
	*sfd = fds[1];

	if (name != NULL) {
		ret = ttyname_r(*mfd, name, (size_t)-1);
		if (ret) {
			return -1;
		}
	}
	if (ios != NULL) {
		ret = tcsetattr(*mfd, TCSANOW, ios);
		if (ret) {
			return -1;
		}
	}
	if (win != NULL) {
		ret = ioctl(*mfd, TIOCGWINSZ, win);
		if (ret) {
			return -1;
		}
	}
	return ret;
}
