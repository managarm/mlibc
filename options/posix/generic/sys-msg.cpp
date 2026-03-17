#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/msg.h>

int msgget(key_t k, int flag) {
	int out = 0;
	if (int e = mlibc::sysdep_or_enosys<Msgget>(k, flag, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
	if (int e = mlibc::sysdep_or_enosys<Msgctl>(msqid, cmd, buf); e) {
		errno = e;
		return -1;
	}

	return 0;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	ssize_t out = 0;
	if (int e = mlibc::sysdep_or_enosys<Msgrcv>(msqid, msgp, msgsz, msgtyp, msgflg, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	if (int e = mlibc::sysdep_or_enosys<Msgsnd>(msqid, msgp, msgsz, msgflg); e) {
		errno = e;
		return -1;
	}

	return 0;
}
