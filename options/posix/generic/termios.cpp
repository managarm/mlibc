#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <errno.h>
#include <termios.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

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
	if(int e = mlibc::sysdep_or_enosys<Tcdrain>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcflow(int fd, int action) {
	if(int e = mlibc::sysdep_or_enosys<Tcflow>(fd, action); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcflush(int fd, int queue_selector) {
	if(int e = mlibc::sysdep_or_enosys<Tcflush>(fd, queue_selector); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcgetattr(int fd, struct termios *attr) {
	if(int e = mlibc::sysdep_or_enosys<Tcgetattr>(fd, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t tcgetsid(int fd) {
	int sid, scratch;
	if(int e = mlibc::sysdep_or_enosys<Ioctl>(fd, TIOCGSID, &sid, &scratch); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int tcsendbreak(int fd, int dur) {
	if(int e = mlibc::sysdep_or_enosys<Tcsendbreak>(fd, dur); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcsetattr(int fd, int opts, const struct termios *attr) {
	if(int e = mlibc::sysdep_or_enosys<Tcsetattr>(fd, opts, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcgetwinsize(int fd, struct winsize *winsz) {
	if(int e = mlibc::sysdep_or_enosys<Tcgetwinsize>(fd, winsz); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int tcsetwinsize(int fd, const struct winsize *winsz) {
	if(int e = mlibc::sysdep_or_enosys<Tcsetwinsize>(fd, winsz); e) {
		errno = e;
		return -1;
	}
	return 0;
}

