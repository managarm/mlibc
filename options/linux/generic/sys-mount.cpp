
#include <errno.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mount, -1);
	if(int e = mlibc::sys_mount(source, target, fstype, flags, data); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int umount(const char *target) {
	return umount2(target, 0);
}

int umount2(const char *target, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_umount2, -1);
	if(int e = mlibc::sys_umount2(target, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsopen(const char *fsname, unsigned int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fsopen, -1);
	if(int e = sysdep(fsname, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsmount(int fsfd, unsigned int flags, unsigned int mountflags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fsmount, -1);
	if(int e = sysdep(fsfd, flags, mountflags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsconfig(int fd, unsigned int cmd, const char *key, const void *val, int aux) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fsconfig, -1);
	if(int e = sysdep(fd, cmd, key, val, aux); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int move_mount(int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_move_mount, -1);
	if(int e = sysdep(from_dirfd, from_path, to_dirfd, to_path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int open_tree(int dirfd, const char *path, unsigned int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_open_tree, -1);
	if(int e = sysdep(dirfd, path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}
