#include <semaphore.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>

static constexpr unsigned int semaphoreHasWaiters = static_cast<uint32_t>(1 << 31);
static constexpr unsigned int semaphoreCountMask = static_cast<uint32_t>(1 << 31) - 1;

int sem_init(sem_t *sem, int pshared, unsigned int initial_count) {
	if (pshared) {
		mlibc::infoLogger() << "mlibc: shared semaphores are unsuppored" << frg::endlog;
		errno = ENOSYS;
		return -1;
	}

	if (initial_count > SEM_VALUE_MAX) {
		errno = EINVAL;
		return -1;
	}

	sem->__mlibc_count = initial_count;

	return 0;
}

int sem_destroy(sem_t *) {
	return 0;
}

int sem_wait(sem_t *sem) {
	unsigned int state = 0;

	while (1) {
		if (!(state & semaphoreCountMask)) {
			if (__atomic_compare_exchange_n(&sem->__mlibc_count, &state, semaphoreHasWaiters,
						false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
				int e = mlibc::sys_futex_wait((int *)&sem->__mlibc_count, state, nullptr);
				if (e == 0 || e == EAGAIN) {
					continue;
				} else if (e == EINTR) {
					errno = EINTR;
					return -1;
				} else {
					mlibc::panicLogger() << "sys_futex_wait() failed with error code " << e << frg::endlog;
				}
			}
		} else {
			unsigned int desired = (state - 1);
			if (__atomic_compare_exchange_n(&sem->__mlibc_count, &state, desired, false,
						__ATOMIC_RELAXED, __ATOMIC_RELAXED))
				return 0;
		}
	}
}

int sem_timedwait(sem_t *sem, const struct timespec *) {
	mlibc::infoLogger() << "\e[31mmlibc: sem_timedwait is implemented as sem_wait\e[0m" << frg::endlog;
	return sem_wait(sem);
}

int sem_post(sem_t *sem) {
	auto old_count = __atomic_load_n(&sem->__mlibc_count, __ATOMIC_RELAXED) & semaphoreCountMask;

	if (old_count + 1 > SEM_VALUE_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	auto state = __atomic_exchange_n(&sem->__mlibc_count, old_count + 1, __ATOMIC_RELEASE);

	if (state & semaphoreHasWaiters)
		if (int e = mlibc::sys_futex_wake((int *)&sem->__mlibc_count); e)
			__ensure(!"sys_futex_wake() failed");

	return 0;
}

sem_t *sem_open(const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sem_close(sem_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sem_getvalue(sem_t *, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sem_unlink(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sem_trywait(sem_t *sem) {
	while (true) {
		auto state = __atomic_load_n(&sem->__mlibc_count, __ATOMIC_ACQUIRE);

		if ((state & semaphoreHasWaiters) || !state) {
			errno = EAGAIN;
			return -1;
		}

		auto desired = state - 1;
		if (__atomic_compare_exchange_n(&sem->__mlibc_count, &state, desired, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
			return 0;
		}
	}
}
