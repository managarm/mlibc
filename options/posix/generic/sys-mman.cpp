
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <bits/ensure.h>

#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int mprotect(void *pointer, size_t size, int prot) {
	if(int e = mlibc::sysdep_or_enosys<VmProtect>(pointer, size, prot); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mlock(const void *addr, size_t len) {
	if(int e = mlibc::sysdep_or_enosys<Mlock>(addr, len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mlockall(int flags) {
	if(int e = mlibc::sysdep_or_enosys<Mlockall>(flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int munlock(const void *addr, size_t len) {
	if(int e = mlibc::sysdep_or_enosys<Munlock>(addr, len); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int munlockall(void) {
	if(int e = mlibc::sysdep_or_enosys<Munlockall>(); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int posix_madvise(void *addr, size_t length, int advice) {
	return mlibc::sysdep_or_enosys<PosixMadvise>(addr, length, advice);
}

int msync(void *addr, size_t length, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Msync>(addr, length, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void *mmap(void *hint, size_t size, int prot, int flags, int fd, off_t offset) {
	void *window;
	if(int e = mlibc::sysdep<VmMap>(hint, size, prot, flags, fd, offset, &window); e) {
		errno = e;
		return (void *)-1;
	}
	return window;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("mmap")]] void *mmap64(void *hint, size_t size, int prot, int flags, int fd, off64_t offset);
#endif /* !__MLIBC_LINUX_OPTION */

int munmap(void *pointer, size_t size) {
	if(int e = mlibc::sysdep<VmUnmap>(pointer, size); e) {
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
			return nullptr;
		}
		if(p - name > NAME_MAX) {
			errno = ENAMETOOLONG;
			return nullptr;
		}
		memcpy(buf, "/dev/shm/", 9);
		memcpy(buf + 9, name, p - name + 1);
		return buf;
	}
} // namespace

int shm_open(const char *name, int flags, mode_t mode) {
	int cs;
	char buf[NAME_MAX + 10];
	if(!(name = shm_mapname(name, buf)))
		return -1;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	int fd = open(name, flags | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, mode);
	pthread_setcancelstate(cs, nullptr);
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
	if(int e = mlibc::sysdep_or_enosys<VmRemap>(pointer, size, new_size, &window); e) {
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

	if(int e = mlibc::sysdep_or_enosys<MemfdCreate>(name, flags, &ret)) {
		errno = e;
		return -1;
	}

	return ret;
}

int madvise(void *addr, size_t length, int advice) {
	if(int e = mlibc::sysdep_or_enosys<Madvise>(addr, length, advice)) {
		errno = e;
		return -1;
	}

	return 0;
}

int mincore(void *addr, size_t length, unsigned char *vec) {
	if(int e = mlibc::sysdep_or_enosys<Mincore>(addr, length, vec); e) {
		errno = e;
		return -1;
	}
	return 0;
}
#endif /* __MLIBC_LINUX_OPTION */
