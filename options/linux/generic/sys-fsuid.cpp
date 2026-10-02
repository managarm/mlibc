#include <errno.h>
#include <sys/fsuid.h>

#include <mlibc/all-sysdeps.hpp>

int setfsuid(uid_t uid) {
	int out;
	if (int e = mlibc::sysdep_or_enosys<SetFsuid>(uid, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}

int setfsgid(gid_t gid) {
	int out;
	if (int e = mlibc::sysdep_or_enosys<SetFsgid>(gid, &out); e) {
		errno = e;
		return -1;
	}

	return out;
}
