#include <asm/ioctls.h>
#include <sys/ioctl.h>

#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

#include <stdio.h>

namespace mlibc {

int sys_isatty(int fd) {
	struct winsize ws;
	long ret = sys_ioctl(fd, TIOCGWINSZ, &ws, 0);

	if(!ret) return 0;

	return ENOTTY;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	if(int e = sys_isatty(fd))
		return e;

	int ret;
	sys_ioctl(fd, TCGETS, attr, &ret);

	if(ret)
		return -ret;

	return 0;
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
	if(int e = sys_isatty(fd))
		return e;

	if(optional_action){
		mlibc::infoLogger() << "mlibc warning: sys_tcsetattr ignores optional_action" << frg::endlog;
	}

	int ret;
	sys_ioctl(fd, TCSETS, const_cast<struct termios*>(attr), &ret);

	if(ret)
		return -ret;

	return 0;
}

int sys_ptsname(int fd, char *buffer, size_t length) {
	int index;
	if(int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

}
