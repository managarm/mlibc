
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int mprotect(void *pointer, size_t size, int prot) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_vm_protect, -1);
	if(int e = mlibc::sys_vm_protect(pointer, size, prot); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mlock(const void *addr, size_t len) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mlock, -1);
	if(int e = mlibc::sys_mlock(addr, len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mlockall(int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mlockall, -1);
	if(int e = mlibc::sys_mlockall(flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int munlock(const void *addr, size_t len) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_munlock, -1);
	if(int e = mlibc::sys_munlock(addr, len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int munlockall(void) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_munlockall, -1);
	if(int e = mlibc::sys_munlockall(); e) {
		errno = e;
		return -1;
	}
	return 0;
}


int posix_madvise(void *, size_t, int) {
	mlibc::infoLogger() << "\e[31m" "mlibc: posix_madvise() fails unconditionally" "\e[39m"
			<< frg::endlog;
	return ENOSYS;
}

int msync(void *addr, size_t length, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_msync, -1);
	if(int e = mlibc::sys_msync(addr, length, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void *mmap(void *hint, size_t size, int prot, int flags, int fd, off_t offset) {
	void *window;
	if(int e = mlibc::sys_vm_map(hint, size, prot, flags, fd, offset, &window); e) {
		errno = e;
		return (void *)-1;
	}
	return window;
}

[[gnu::alias("mmap")]] void *mmap64(void *hint, size_t size, int prot, int flags, int fd, off64_t offset);

int munmap(void *pointer, size_t size) {
	if(int e = mlibc::sys_vm_unmap(pointer, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// The implementation of shm_open and shm_unlink is taken from musl.
namespace {
	char *shm_mapname(const char *name, char *buf) {
		char *p;
		while(*name == '/')
			name++;
		if(*(p = strchrnul(name, '/')) || p == name ||
			(p - name <= 2 && name[0] == '.' && p[-1] == '.')) {
			errno = EINVAL;
			return 0;
		}
		if(p - name > NAME_MAX) {
			errno = ENAMETOOLONG;
			return 0;
		}
		memcpy(buf, "/dev/shm/", 9);
		memcpy(buf + 9, name, p - name + 1);
		return buf;
	}
}

int shm_open(const char *name, int flags, mode_t mode) {
	int cs;
	char buf[NAME_MAX + 10];
	if(!(name = shm_mapname(name, buf)))
		return -1;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	int fd = open(name, flags | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, mode);
	pthread_setcancelstate(cs, 0);
	return fd;
}

int shm_unlink(const char *name) {
	char buf[NAME_MAX + 10];
	if(!(name = shm_mapname(name, buf)))
		return -1;
	return unlink(name);
}

#if __MLIBC_LINUX_OPTION
void *mremap(void *pointer, size_t size, size_t new_size, int flags, ...) {
	__ensure(flags == MREMAP_MAYMOVE);

	void *window;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_vm_remap, (void *)-1);
	if(int e = mlibc::sys_vm_remap(pointer, size, new_size, &window); e) {
		errno = e;
		return (void *)-1;
	}
	return window;
}

int remap_file_pages(void *, size_t, int, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int memfd_create(const char *name, unsigned int flags) {
	int ret = -1;

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_memfd_create, -1);
	if(int e = mlibc::sys_memfd_create(name, flags, &ret)) {
		errno = e;
		return -1;
	}

	return ret;
}

int madvise(void *addr, size_t length, int advice) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_madvise, -1);
	if(int e = mlibc::sys_madvise(addr, length, advice)) {
		errno = e;
		return -1;
	}

	return 0;
}

int mincore(void *addr, size_t length, unsigned char *vec) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_munlockall, -1);
	if(int e = mlibc::sys_mincore(addr, length, vec); e) {
		errno = e;
		return -1;
	}
	return 0;
}
#endif /* __MLIBC_LINUX_OPTION */
