#include <bit>
#include <type_traits>

#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc-config.h>

#include <mlibc/posix-sysdeps.hpp>

using fd_mask_unsigned = std::make_unsigned_t<__fd_mask>;

#define FD_MASK(n) (fd_mask_unsigned)(fd_mask_unsigned{1} << (n))

void __FD_CLR(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->fds_bits[fd / NFDBITS] &= std::bit_cast<fd_mask>(~FD_MASK(fd % NFDBITS));
}
int __FD_ISSET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	return (std::bit_cast<fd_mask_unsigned>(set->fds_bits[fd / NFDBITS]) & FD_MASK(fd % NFDBITS)) != 0;
}
void __FD_SET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->fds_bits[fd / NFDBITS] = std::bit_cast<fd_mask>(std::bit_cast<fd_mask_unsigned>(set->fds_bits[fd / NFDBITS]) | FD_MASK(fd % NFDBITS));
}
void __FD_ZERO(fd_set *set) {
	memset(set->fds_bits, 0, sizeof(fd_set));
}

int select(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, struct timeval *__restrict timeout) {
	int num_events = 0;
	struct timespec timeouts = {};
	struct timespec *timeout_ptr = nullptr;
	if (timeout) {
		timeouts.tv_sec = timeout->tv_sec;
		timeouts.tv_nsec = timeout->tv_usec * 1000;
		timeout_ptr = &timeouts;
	}

    MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pselect, -1);
	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout_ptr, nullptr, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}

int pselect(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, const struct timespec *timeout, const sigset_t *sigmask) {
	int num_events = 0;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pselect, -1);
	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout, sigmask, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}
