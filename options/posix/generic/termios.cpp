#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <errno.h>
#include <termios.h>

#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>

speed_t cfgetispeed(const struct termios *tios) {
	return tios->c_cflag & CBAUD;
}

speed_t cfgetospeed(const struct termios *tios) {
	return tios->c_cflag & CBAUD;
}

int cfsetispeed(struct termios *termios, speed_t speed) {
	return speed ? cfsetospeed(termios, speed) : 0;
}

int cfsetospeed(struct termios *termios, speed_t speed) {
	if(speed & ~CBAUD) {
		errno = EINVAL;
		return -1;
	}

	termios->c_cflag &= ~CBAUD;
	termios->c_cflag |= speed;

	return 0;
}

void cfmakeraw(struct termios *t) {
	t->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	t->c_oflag &= ~OPOST;
	t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	t->c_cflag &= ~(CSIZE | PARENB);
	t->c_cflag |= CS8;
	t->c_cc[VMIN] = 1;
	t->c_cc[VTIME] = 0;
}

int tcdrain(int fd) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcdrain, -1);
	if(int e = mlibc::sys_tcdrain(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcflow(int fd, int action) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcflow, -1);
	if(int e = mlibc::sys_tcflow(fd, action); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcflush(int fd, int queue_selector) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcflush, -1);
	if(int e = mlibc::sys_tcflush(fd, queue_selector); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcgetattr(int fd, struct termios *attr) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcgetattr, -1);
	if(int e = mlibc::sys_tcgetattr(fd, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t tcgetsid(int fd) {
	int sid, scratch;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ioctl, -1);
	if(int e = mlibc::sys_ioctl(fd, TIOCGSID, &sid, &scratch); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int tcsendbreak(int fd, int dur) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcsendbreak, -1);
	if(int e = sysdep(fd, dur); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcsetattr(int fd, int opts, const struct termios *attr) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcsetattr, -1);
	if(int e = mlibc::sys_tcsetattr(fd, opts, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcgetwinsize(int fd, struct winsize *winsz) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcgetwinsize, -1);
	if(int e = sysdep(fd, winsz); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcsetwinsize(int fd, const struct winsize *winsz) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tcsetwinsize, -1);
	if(int e = sysdep(fd, winsz); e) {
		errno = e;
		return -1;
	}
	return 0;
}

