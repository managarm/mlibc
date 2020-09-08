
#include <errno.h>
#include <termios.h>

#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>

speed_t cfgetispeed(const struct termios *tios) {
	return tios->ibaud;
}
speed_t cfgetospeed(const struct termios *tios) {
	return tios->obaud;
}
int cfsetispeed(struct termios *, speed_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int cfsetospeed(struct termios *, speed_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void cfmakeraw(struct termios *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcdrain(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcflow(int fd, int action) {
	if(!mlibc::sys_tcflow) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_tcflow(fd, action); e) {
		errno = e;
		return -1;
	}
	return 0;
}
int tcflush(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int tcgetattr(int fd, struct termios *attr) {
	if(!mlibc::sys_tcgetattr) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_tcgetattr(fd, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t tcgetsid(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcsendbreak(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int tcsetattr(int fd, int opts, const struct termios *attr) {
	if(!mlibc::sys_tcsetattr) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_tcsetattr(fd, opts, attr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

