
#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	__ensure(!ios);

	if (win) {
		mlibc::infoLogger() << "mlibc: openpty ignores win argument" << frg::endlog;
	}

	// FIXME: Close the master FD if the slave open fails.

	int ptmx_fd;
	if(int e = mlibc::sys_open("/dev/ptmx", O_RDWR | O_NOCTTY, 0, &ptmx_fd); e) {
		errno = e;
		return -1;
	}

	char spath[32];
	if(!name)
		name = spath;
	if(ptsname_r(ptmx_fd, name, 32))
		return -1;

	int pts_fd;
	unlockpt(ptmx_fd);
	if(int e = mlibc::sys_open(spath, O_RDWR | O_NOCTTY, 0, &pts_fd); e) {
		errno = e;
		return -1;
	}

	*mfd = ptmx_fd;
	*sfd = pts_fd;
	return 0;
}

int login_tty(int fd) {
	if(setsid() == -1)
		return -1;
	if(ioctl(fd, TIOCSCTTY, 0))
		return -1;

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_dup2, -1);
	if(int e = mlibc::sys_dup2(fd, 0, STDIN_FILENO); e) {
		errno = e;
		return -1;
	}
	if(int e = mlibc::sys_dup2(fd, 0, STDOUT_FILENO); e) {
		errno = e;
		return -1;
	}
	if(int e = mlibc::sys_dup2(fd, 0, STDERR_FILENO); e) {
		errno = e;
		return -1;
	}

	if(int e = mlibc::sys_close(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int forkpty(int *mfd, char *name, const struct termios *ios, const struct winsize *win) {
	int sfd;
	if(openpty(mfd, &sfd, name, ios, win))
		return -1;

	pid_t child;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fork, -1);
	if(int e = mlibc::sys_fork(&child); e) {
		errno = e;
		return -1;
	}

	if(!child) {
		if(login_tty(sfd))
			mlibc::panicLogger() << "mlibc: TTY login fail in forkpty() child" << frg::endlog;
	}else{
		if(int e = mlibc::sys_close(sfd); e) {
			errno = e;
			return -1;
		}
	}

	return child;
}

