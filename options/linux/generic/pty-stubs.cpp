
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
	if(mlibc::sys_open("/dev/ptmx", O_RDWR | O_NOCTTY, &ptmx_fd))
		return -1;

	char spath[32];
	if(ptsname_r(ptmx_fd, spath, 32))
		return -1;
	
	int pts_fd;
	if(mlibc::sys_open(spath, O_RDWR | O_NOCTTY, &pts_fd))
		return -1;
	
	*mfd = ptmx_fd;
	*sfd = pts_fd;
	return 0;
}

int login_tty(int fd) {
	// TODO: Perform an ioctl() to set the controlling terminal.

	if(mlibc::sys_dup2(fd, 0, STDIN_FILENO)
			|| mlibc::sys_dup2(fd, 0, STDOUT_FILENO)
			|| mlibc::sys_dup2(fd, 0, STDERR_FILENO))
		return -1;

	if(mlibc::sys_close(fd))
		return -1;
	return 0;
}

int forkpty(int *mfd, char *name, const struct termios *ios, const struct winsize *win) {
	int sfd;
	if(openpty(mfd, &sfd, name, ios, win))
		return -1;

	pid_t child;
	if(mlibc::sys_fork(&child))
		return -1;

	if(!child) {
		if(login_tty(sfd))
			mlibc::panicLogger() << "mlibc: TTY login fail in forkpty() child" << frg::endlog;
	}else{
		if(mlibc::sys_close(sfd))
			return -1;
	}

	return child;
}

