#include <dirent.h>
#include <fcntl.h>
#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

#include <stddef.h>

static_assert(sizeof(roxy_dirent) == sizeof(struct dirent));
static_assert(offsetof(roxy_dirent, inode) == offsetof(struct dirent, d_ino));
static_assert(offsetof(roxy_dirent, offset) == offsetof(struct dirent, d_off));
static_assert(offsetof(roxy_dirent, record_size) == offsetof(struct dirent, d_reclen));
static_assert(offsetof(roxy_dirent, type) == offsetof(struct dirent, d_type));
static_assert(offsetof(roxy_dirent, name) == offsetof(struct dirent, d_name));

namespace mlibc {

int Sysdeps<Chdir>::operator()(const char *path) {
	auto result = roxy_syscall1(ROXY_SYS_CHDIR, reinterpret_cast<long>(path));

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	auto result = roxy_syscall2(
	    ROXY_SYS_GETCWD,
	    reinterpret_cast<long>(buffer),
	    size
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	auto result = roxy_syscall1(ROXY_SYS_OPEN_DIR, reinterpret_cast<long>(path));
	if(result < 0)
		return static_cast<int>(-result);

	*handle = static_cast<int>(result);
	return 0;
}

int Sysdeps<ReadEntries>::operator()(
	int handle,
	void *buffer,
	size_t max_size,
	size_t *bytes_read
) {
	auto result = roxy_syscall3(
	    ROXY_SYS_READ_ENTRIES,
	    handle,
	    reinterpret_cast<long>(buffer),
	    max_size
	);
	if(result < 0)
		return static_cast<int>(-result);

	*bytes_read = static_cast<size_t>(result);
	return 0;
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	return sysdep<Mkdirat>(AT_FDCWD, path, mode);
}

int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	auto result = roxy_syscall3(
	    ROXY_SYS_MKDIRAT,
	    dirfd,
	    reinterpret_cast<long>(path),
	    mode
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	return sysdep<Unlinkat>(AT_FDCWD, path, AT_REMOVEDIR);
}

int Sysdeps<Unlinkat>::operator()(int dirfd, const char *path, int flags) {
	auto result = roxy_syscall3(
	    ROXY_SYS_UNLINKAT,
	    dirfd,
	    reinterpret_cast<long>(path),
	    flags
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Readlink>::operator()(
	const char *path,
	void *buffer,
	size_t max_size,
	ssize_t *length
) {
	return sysdep<Readlinkat>(AT_FDCWD, path, buffer, max_size, length);
}

int Sysdeps<Readlinkat>::operator()(
	int dirfd,
	const char *path,
	void *buffer,
	size_t max_size,
	ssize_t *length
) {
	auto result = roxy_syscall4(
	    ROXY_SYS_READLINKAT,
	    dirfd,
	    reinterpret_cast<long>(path),
	    reinterpret_cast<long>(buffer),
	    max_size
	);
	if(result < 0)
		return static_cast<int>(-result);

	*length = static_cast<ssize_t>(result);
	return 0;
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int Sysdeps<Linkat>::operator()(
	int olddirfd,
	const char *old_path,
	int newdirfd,
	const char *new_path,
	int flags
) {
	auto result = roxy_syscall5(
	    ROXY_SYS_LINKAT,
	    olddirfd,
	    reinterpret_cast<long>(old_path),
	    newdirfd,
	    reinterpret_cast<long>(new_path),
	    flags
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	auto result = roxy_syscall3(
	    ROXY_SYS_SYMLINKAT,
	    reinterpret_cast<long>(target_path),
	    dirfd,
	    reinterpret_cast<long>(link_path)
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
	return sysdep<Renameat>(AT_FDCWD, path, AT_FDCWD, new_path);
}

int Sysdeps<Renameat>::operator()(
	int olddirfd,
	const char *old_path,
	int newdirfd,
	const char *new_path
) {
	auto result = roxy_syscall4(
	    ROXY_SYS_RENAMEAT,
	    olddirfd,
	    reinterpret_cast<long>(old_path),
	    newdirfd,
	    reinterpret_cast<long>(new_path)
	);

	return result < 0 ? static_cast<int>(-result) : 0;
}

void Sysdeps<Sync>::operator()() {
	roxy_syscall0(ROXY_SYS_SYNC);
}

int Sysdeps<Fsync>::operator()(int fd) {
	auto result = roxy_syscall1(ROXY_SYS_FSYNC, fd);

	return result < 0 ? static_cast<int>(-result) : 0;
}

int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
	auto result = roxy_syscall2(ROXY_SYS_FTRUNCATE, fd, size);

	return result < 0 ? static_cast<int>(-result) : 0;
}

} // namespace mlibc
