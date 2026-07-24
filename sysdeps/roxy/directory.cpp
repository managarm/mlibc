#include <dirent.h>
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

} // namespace mlibc
