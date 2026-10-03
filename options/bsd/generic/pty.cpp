
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <utmp.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/dlapi.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/tid.hpp>

int openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	if constexpr (mlibc::IsImplemented<Openpty>) {
		if(int e = mlibc::sysdep_or_enosys<Openpty>(mfd, sfd, name, ios, win); e) {
			errno = e;
			return -1;
		}
		return 0;
	}

	int ptmx_fd;
	if(int e = mlibc::sysdep<Open>("/dev/ptmx", O_RDWR | O_NOCTTY, 0, &ptmx_fd); e) {
		errno = e;
		goto fail_noclose;
	}

	char spath[TTY_NAME_MAX];
	if(!name)
		name = spath;
	if(ptsname_r(ptmx_fd, name, TTY_NAME_MAX))
		goto fail;

	int pts_fd;
	unlockpt(ptmx_fd);
	if(int e = mlibc::sysdep<Open>(name, O_RDWR | O_NOCTTY, 0, &pts_fd); e) {
		errno = e;
		goto fail;
	}

	if(ios)
		tcsetattr(ptmx_fd, TCSAFLUSH, ios);

	if(win)
		ioctl(ptmx_fd, TIOCSWINSZ, (void*)win);

	*mfd = ptmx_fd;
	*sfd = pts_fd;
	return 0;

fail:
	mlibc::sysdep<Close>(ptmx_fd);
fail_noclose:
	return -1;
}

int login_tty(int fd) {
	if(setsid() == -1)
		return -1;
	if(ioctl(fd, TIOCSCTTY, 0))
		return -1;

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Dup2>, -1);
	if(int e = mlibc::sysdep<Dup2>(fd, 0, STDIN_FILENO); e) {
		errno = e;
		return -1;
	}
	if(int e = mlibc::sysdep<Dup2>(fd, 0, STDOUT_FILENO); e) {
		errno = e;
		return -1;
	}
	if(int e = mlibc::sysdep<Dup2>(fd, 0, STDERR_FILENO); e) {
		errno = e;
		return -1;
	}

	if(int e = mlibc::sysdep<Close>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int forkpty(int *mfd, char *name, const struct termios *ios, const struct winsize *win) {
	int sfd;
	if(openpty(mfd, &sfd, name, ios, win))
		return -1;

	auto self = mlibc::get_current_tcb();
	auto parent_tid = self->tid;
	pid_t child;
	__dlapi_prefork();
	if(int e = mlibc::sysdep_or_enosys<Fork>(&child); e) {
		__dlapi_postfork_finish();
		errno = e;
		return -1;
	}

	if(!child) {
		// update the cached TID in the TCB
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);
		__dlapi_postfork_rebind(parent_tid);
		__dlapi_postfork_finish();

		if(login_tty(sfd))
			mlibc::panicLogger() << "mlibc: TTY login fail in forkpty() child" << frg::endlog;
	}else{
		__dlapi_postfork_finish();
		if(int e = mlibc::sysdep<Close>(sfd); e) {
			errno = e;
			return -1;
		}
	}

	return child;
}
