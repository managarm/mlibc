
#include <errno.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	if(int e = mlibc::sysdep_or_enosys<Mount>(source, target, fstype, flags, data); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int umount(const char *target) {
	return umount2(target, 0);
}

int umount2(const char *target, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Umount2>(target, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsopen(const char *fsname, unsigned int flags) {
	int outfd = 0;
	if(int e = mlibc::sysdep_or_enosys<Fsopen>(fsname, flags, &outfd); e) {
		errno = e;
		return -1;
	}
	return outfd;
}

int fsmount(int fsfd, unsigned int flags, unsigned int mountflags) {
	int outfd = 0;
	if(int e = mlibc::sysdep_or_enosys<Fsmount>(fsfd, flags, mountflags, &outfd); e) {
		errno = e;
		return -1;
	}
	return outfd;
}

int fsconfig(int fd, unsigned int cmd, const char *key, const void *val, int aux) {
	if(int e = mlibc::sysdep_or_enosys<Fsconfig>(fd, cmd, key, val, aux); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int move_mount(int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags) {
	if(int e = mlibc::sysdep_or_enosys<MoveMount>(from_dirfd, from_path, to_dirfd, to_path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int open_tree(int dirfd, const char *path, unsigned int flags) {
	int out_fd;
	if(int e = mlibc::sysdep_or_enosys<OpenTree>(dirfd, path, flags, &out_fd); e) {
		errno = e;
		return -1;
	}
	return out_fd;
}
