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
		name = ttyname(*mfd);
		if (!name) {
			return -1;
		}
	}

	if (ios == NULL) {
		struct termios termios;
		termios.c_iflag = BRKINT | IGNPAR | ICRNL | IXON | IMAXBEL;
		termios.c_oflag = OPOST | ONLCR;
		termios.c_cflag = CS8 | CREAD;
		termios.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE;
		termios.c_cc[VINTR] = CTRL('C');
		termios.c_cc[VERASE] = 127; // Delete.
		termios.c_cc[VEOF] = CTRL('D');
		termios.c_cc[VSUSP] = CTRL('Z');
		termios.ibaud = 38400;
		termios.obaud = 38400;
		ret = tcsetattr(*mfd, TCSANOW, &termios);
	} else {
		ret = tcsetattr(*mfd, TCSANOW, ios);
	}
	if (ret) {
		return -1;
	}

	if (win == NULL) {
		struct winsize win_size = {
			.ws_row = 24,
			.ws_col = 80,
			.ws_xpixel = 24 * 16,
			.ws_ypixel = 80 * 16
		};
		ret = ioctl(*mfd, TIOCSWINSZ, &win_size);
	} else {
		ret = ioctl(*mfd, TIOCSWINSZ, win);
	}

	if (ret) {
		return -1;
	}

	return ret;
}
