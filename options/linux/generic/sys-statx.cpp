#include <errno.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/linux-sysdeps.hpp>

int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
	if(!mlibc::sys_statx) {
		struct stat statbuf;
		MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, -1);

		if (!(flags & AT_NO_AUTOMOUNT)) {
			mlibc::infoLogger()
				<< "mlibc: sys_statx is unavailable, and stat does not support not specifying AT_NO_MOUNTPOINT"
				<< frg::endlog;
		}

		// AT_STATX_SYNC_AS_STAT is the default and behaves like good old stat
		if ((flags & AT_STATX_FORCE_SYNC) || (flags & AT_STATX_DONT_SYNC)) {
			mlibc::infoLogger()
				<< "mlibc: sys_statx is unavailable, and stat does not support modes other than AT_STATX_SYNC_AS_STAT"
				<< frg::endlog;
		}

		// Mask out flags not appropriate for regular stat
		flags &= ~(AT_NO_AUTOMOUNT | AT_STATX_SYNC_AS_STAT | AT_STATX_FORCE_SYNC | AT_STATX_DONT_SYNC);

		if(int e = mlibc::sys_stat(mlibc::fsfd_target::fd_path, dirfd, pathname, flags, &statbuf); e) {
			errno = e;
			return -1;
		}

		memset(statxbuf, 0, sizeof(struct statx));
		statxbuf->stx_blksize = statbuf.st_blksize;
		statxbuf->stx_blocks = statbuf.st_blocks;
		statxbuf->stx_gid = statbuf.st_gid;
		statxbuf->stx_ino = statbuf.st_ino;
		statxbuf->stx_mode = statbuf.st_mode;
		statxbuf->stx_nlink = statbuf.st_nlink;
		statxbuf->stx_size = statbuf.st_size;
		statxbuf->stx_uid = statbuf.st_uid;

		statxbuf->stx_atime.tv_sec = statbuf.st_atim.tv_sec;
		statxbuf->stx_atime.tv_nsec = statbuf.st_atim.tv_nsec;
		statxbuf->stx_btime.tv_sec = statbuf.st_mtim.tv_sec;
		statxbuf->stx_btime.tv_nsec = statbuf.st_mtim.tv_nsec;
		statxbuf->stx_ctime.tv_sec = statbuf.st_ctim.tv_sec;
		statxbuf->stx_ctime.tv_nsec = statbuf.st_ctim.tv_nsec;
		statxbuf->stx_mtime.tv_sec = statbuf.st_mtim.tv_sec;
		statxbuf->stx_mtime.tv_nsec = statbuf.st_mtim.tv_nsec;

		statxbuf->stx_rdev_major = major(statbuf.st_rdev);
		statxbuf->stx_rdev_minor = minor(statbuf.st_rdev);
		statxbuf->stx_dev_major = major(statbuf.st_dev);
		statxbuf->stx_dev_minor = minor(statbuf.st_dev);
		statxbuf->stx_mask = STATX_BASIC_STATS | STATX_BTIME;

		return 0;
	}

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_statx, -1);
	if(int e = sysdep(dirfd, pathname, flags, mask, statxbuf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

