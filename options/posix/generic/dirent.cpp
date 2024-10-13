
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/debug.hpp>

// Code taken from musl
int alphasort(const struct dirent **a, const struct dirent **b) {
	return strcoll((*a)->d_name, (*b)->d_name);
}

int closedir(DIR *dir) {
	// TODO: Deallocate the dir structure.
	close(dir->__handle);
	return 0;
}
int dirfd(DIR *dir) {
	return dir->__handle;
}
DIR *fdopendir(int fd) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		return nullptr;
	}
	// Musl implements this, but O_PATH is only declared on the linux abi
	/*if(fcntl(fd, F_GETFL) & O_PATH) {
		errno = EBADF;
		return nullptr;
	}*/
	if(!S_ISDIR(st.st_mode)) {
		errno = ENOTDIR;
		return nullptr;
	}
	auto dir = frg::construct<__mlibc_dir_struct>(getAllocator());
	__ensure(dir);
	dir->__ent_next = 0;
	dir->__ent_limit = 0;
	int flags = fcntl(fd, F_GETFD);
	fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
	dir->__handle = fd;
	return dir;
}
DIR *opendir(const char *path) {
	auto dir = frg::construct<__mlibc_dir_struct>(getAllocator());
	__ensure(dir);
	dir->__ent_next = 0;
	dir->__ent_limit = 0;

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_open_dir, nullptr);
	if(int e = mlibc::sys_open_dir(path, &dir->__handle); e) {
		errno = e;
		frg::destruct(getAllocator(), dir);
		return nullptr;
	}else{
		return dir;
	}
}

struct dirent *readdir(DIR *dir) {
	__ensure(dir->__ent_next <= dir->__ent_limit);
	if(dir->__ent_next == dir->__ent_limit) {
		MLIBC_CHECK_OR_ENOSYS(mlibc::sys_read_entries, nullptr);
		if(int e = mlibc::sys_read_entries(dir->__handle, dir->__ent_buffer, 2048, &dir->__ent_limit); e)
			__ensure(!"mlibc::sys_read_entries() failed");
		dir->__ent_next = 0;
		if(!dir->__ent_limit)
			return nullptr;
	}

	auto entp = reinterpret_cast<struct dirent *>(dir->__ent_buffer + dir->__ent_next);
	// We only copy as many bytes as we need to avoid buffer-overflows.
	memcpy(&dir->__current, entp, offsetof(struct dirent, d_name) + strlen(entp->d_name) + 1);
	dir->__ent_next += entp->d_reclen;
	return &dir->__current;
}

[[gnu::alias("readdir")]] struct dirent64 *readdir64(DIR *dir);

int readdir_r(DIR *dir, struct dirent *entry, struct dirent **result) {
	if(!mlibc::sys_read_entries) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}

	__ensure(dir->__ent_next <= dir->__ent_limit);
	if(dir->__ent_next == dir->__ent_limit) {
		if(int e = mlibc::sys_read_entries(dir->__handle, dir->__ent_buffer, 2048, &dir->__ent_limit); e)
			__ensure(!"mlibc::sys_read_entries() failed");
		dir->__ent_next = 0;
		if(!dir->__ent_limit) {
			*result = NULL;
			return 0;
		}
	}

	auto entp = reinterpret_cast<struct dirent *>(dir->__ent_buffer + dir->__ent_next);
	// We only copy as many bytes as we need to avoid buffer-overflows.
	memcpy(entry, entp, offsetof(struct dirent, d_name) + strlen(entp->d_name) + 1);
	dir->__ent_next += entp->d_reclen;
	*result = entry;
	return 0;
}

void rewinddir(DIR *dir) {
	lseek(dir->__handle, 0, SEEK_SET);
	dir->__ent_next = 0;
}

int scandir(const char *path, struct dirent ***res, int (*select)(const struct dirent *),
		int (*compare)(const struct dirent **, const struct dirent **)) {
	DIR *dir = opendir(path);
	if (!dir)
		return -1; // errno will be set by opendir()

	// we should save the errno
	int old_errno = errno;
	errno = 0;

	struct dirent *dir_ent;
	struct dirent **array = nullptr, **tmp = nullptr;
	int length = 0;
	int count = 0;
	while((dir_ent = readdir(dir)) && !errno) {
		if(select && !select(dir_ent))
			continue;

		if(count >= length) {
			length = 2*length + 1;
			tmp = static_cast<struct dirent**>(realloc(array,
						length * sizeof(struct dirent*)));
			// we need to check the call actually goes through
			// before we overwrite array so that we can
			// deallocate the already written entries should realloc()
			// have failed
			if(!tmp)
				break;
			array = tmp;
		}
		array[count] = static_cast<struct dirent*>(malloc(dir_ent->d_reclen));
		if(!array[count])
			break;

		memcpy(array[count], dir_ent, dir_ent->d_reclen);
		count++;
	}

	if(errno) {
		if(array)
			while(count-- > 0)
				free(array[count]);
		free(array);
		return -1;
	}

	// from here we can set the old errno back
	errno = old_errno;

	if(compare)
		qsort(array, count, sizeof(struct dirent*),
				(int (*)(const void *, const void *)) compare);
	*res = array;
	return count;
}
void seekdir(DIR *, long) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long telldir(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int versionsort(const struct dirent **a, const struct dirent **b) {
	return strverscmp((*a)->d_name, (*b)->d_name);
}
