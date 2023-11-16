#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	int errno, ret;
	int fds[2];
	SYSCALL3(SYSCALL_OPENPTY, fds, ios, win);
	*mfd = fds[0];
	*sfd = fds[1];
	return ret;
}
