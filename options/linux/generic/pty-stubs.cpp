
#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <unistd.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int ptsname_r(int fd, char *buffer, size_t length) {
	int index;
	if(ioctl(fd, TIOCGPTN, &index))
		return -1;
	if(snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		errno = ERANGE;
		return -1;
	}
	return 0;
}

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	__ensure(!name);
	__ensure(!ios);
	__ensure(!win);

	// FIXME: Close the master FD if the slave open fails.

	int ptmx_fd;
	if(int e = mlibc::sys_open("/dev/ptmx", O_RDWR | O_NOCTTY, &ptmx_fd); e) {
		errno = e;
		return -1;
	}

	char spath[32];
	if(ptsname_r(ptmx_fd, spath, 32))
		return -1;
	
	int pts_fd;
	if(int e = mlibc::sys_open(spath, O_RDWR | O_NOCTTY, &pts_fd); e) {
		errno = e;
		return -1;
	}
	
	*mfd = ptmx_fd;
	*sfd = pts_fd;
	return 0;
}

int login_tty(int fd) {
	// TODO: Perform an ioctl() to set the controlling terminal.
	int e;
	if(!mlibc::sys_dup2) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(e = mlibc::sys_dup2(fd, 0, STDIN_FILENO)) {
		errno = e;
		return -1;
	}
	if(e = mlibc::sys_dup2(fd, 0, STDOUT_FILENO)) {
		errno = e;
		return -1;
	}
	if(e = mlibc::sys_dup2(fd, 0, STDERR_FILENO)) {
		errno = e;
		return -1;
	}

	if(e = mlibc::sys_close(fd); e) {
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
	if(!mlibc::sys_fork) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
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

