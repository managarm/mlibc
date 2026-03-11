#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/posix-sysdeps.hpp>
#include <sys/msg.h>

int msgget(key_t k, int flag) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msgget, ENOSYS);
	int out = 0;
	if (int e = sysdep(k, flag, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msgctl, ENOSYS);
	if (int e = sysdep(msqid, cmd, buf); e) {
		errno = e;
		return -1;
	}

	return 0;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msgrcv, ENOSYS);
	ssize_t out = 0;
	if (int e = sysdep(msqid, msgp, msgsz, msgtyp, msgflg, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msgsnd, ENOSYS);
	if (int e = sysdep(msqid, msgp, msgsz, msgflg); e) {
		errno = e;
		return -1;
	}

	return 0;
}
