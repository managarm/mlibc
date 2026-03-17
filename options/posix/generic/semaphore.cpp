#include <semaphore.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/time-helpers.hpp>

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
				int e = mlibc::sysdep<FutexWait>((int *)&sem->__mlibc_count, state, nullptr);
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

int sem_timedwait(sem_t *sem, const struct timespec *abstime) {
	return sem_clockwait(sem, CLOCK_REALTIME, abstime);
}

int sem_clockwait(sem_t *sem, clockid_t clockid, const struct timespec *abstime) {
	unsigned int state = 0;

	while (1) {
		if (!(state & semaphoreCountMask)) {
			if (__atomic_compare_exchange_n(
			        &sem->__mlibc_count,
			        &state,
			        semaphoreHasWaiters,
			        false,
			        __ATOMIC_ACQUIRE,
			        __ATOMIC_ACQUIRE
			    )) {
				// Adjust for the fact that sys_futex_wait accepts a *timeout*, but
				// we accept an *absolute time*.
				struct timespec timeout;
				if (!mlibc::time_absolute_to_relative(clockid, abstime, &timeout)) {
					errno = EINVAL;
					return -1;
				}

				if (timeout.tv_sec == 0 && timeout.tv_nsec == 0) {
					errno = ETIMEDOUT;
					return -1;
				}

				int e = mlibc::sysdep<FutexWait>((int *)&sem->__mlibc_count, state, &timeout);

				if (e == 0 || e == EAGAIN) {
					continue;
				} else if (e == EINTR || e == ETIMEDOUT) {
					errno = e;
					return -1;
				} else {
					mlibc::panicLogger()
					    << "sys_futex_wait() failed with error code " << e << frg::endlog;
				}
			}
		} else {
			unsigned int desired = (state - 1);
			if (__atomic_compare_exchange_n(
			        &sem->__mlibc_count, &state, desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED
			    ))
				return 0;
		}
	}
}

int sem_post(sem_t *sem) {
	auto old_count = __atomic_load_n(&sem->__mlibc_count, __ATOMIC_RELAXED) & semaphoreCountMask;

	if (old_count + 1 > SEM_VALUE_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	auto state = __atomic_exchange_n(&sem->__mlibc_count, old_count + 1, __ATOMIC_RELEASE);

	if (state & semaphoreHasWaiters)
		if (int e = mlibc::sysdep<FutexWake>((int *)&sem->__mlibc_count, true); e)
			__ensure(!"sys_futex_wake() failed");

	return 0;
}

sem_t *sem_open(const char *, int, ...) {
	errno = ENOSYS;
	return SEM_FAILED;
}

int sem_close(sem_t *) {
	errno = ENOSYS;
	return -1;
}

int sem_getvalue(sem_t *, int *) {
	errno = ENOSYS;
	return -1;
}

int sem_unlink(const char *) {
	errno = ENOSYS;
	return -1;
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
