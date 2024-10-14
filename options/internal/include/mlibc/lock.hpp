#ifndef MLIBC_LOCK_HPP
#define MLIBC_LOCK_HPP

#include <errno.h>
#include <stdint.h>
#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tid.hpp>
#include <bits/ensure.h>

// alignas(4) is specified for the benefit of m68k, where default alignment is
// 2 bytes for a uint32_t, while the futex syscall requires 4-byte alignment.
// It is a no-op on any other architecture.

template<bool Recursive>
struct alignas(4) FutexLockImpl {
	FutexLockImpl() : _state{0}, _recursion{0} { }

	FutexLockImpl(const FutexLockImpl &) = delete;

	FutexLockImpl &operator= (const FutexLockImpl &) = delete;

	static constexpr uint32_t waitersBit = (1 << 31);
	static constexpr uint32_t ownerMask = (static_cast<uint32_t>(1) << 30) - 1;

	void lock() {
		unsigned int this_tid = mlibc::this_tid();
		unsigned int expected = 0;

		while(true) {
			if(!expected) {
				// Try to take the mutex here.
				if(__atomic_compare_exchange_n(&_state,
						&expected, this_tid, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
					if constexpr (Recursive) {
						__ensure(!_recursion);
						_recursion = 1;
					}
					return;
				}
			}else{
				// If this (recursive) mutex is already owned by us, increment the recursion level.
				if((expected & ownerMask) == this_tid) {
					if constexpr (Recursive)
						++_recursion;
					else
						mlibc::panicLogger() << "mlibc: FutexLock deadlock detected!" << frg::endlog;
					return;
				}

				// Wait on the futex if the waiters flag is set.
				if(expected & waitersBit) {
					int e = mlibc::sys_futex_wait((int *)&_state, expected, nullptr);

					// If the wait returns EAGAIN, that means that the waitersBit was just unset by
					// some other thread. In this case, we should loop back around.
					// Also loop around in case of a signal interrupting the wait
					if (e && e != EAGAIN && e != EINTR)
						mlibc::panicLogger() << "sys_futex_wait() failed with error code " << e << frg::endlog;

					// Opportunistically try to take the lock after we wake up.
					expected = 0;
				}else{
					// Otherwise we have to set the waiters flag first.
					unsigned int desired = expected | waitersBit;
					if(__atomic_compare_exchange_n((int *)&_state,
							reinterpret_cast<int*>(&expected), desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
						expected = desired;
				}
			}
		}
	}

	bool try_lock() {
		unsigned int this_tid = mlibc::this_tid();
		unsigned int expected = __atomic_load_n(&_state, __ATOMIC_RELAXED);

		if(!expected) {
			// Try to take the mutex here.
			if(__atomic_compare_exchange_n(&_state,
							&expected, this_tid, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
				if constexpr (Recursive)
					_recursion = 1;
				return true;
			}
		} else {
			// If this (recursive) mutex is already owned by us, increment the recursion level.
			if((expected & ownerMask) == this_tid) {
				if constexpr (Recursive) {
					__ensure(!_recursion);
					++_recursion;
					return true;
				} else {
					return false;
				}
			}
		}

		return false;
	}

	void unlock() {
		// Decrement the recursion level and unlock if we hit zero.
		if constexpr (Recursive) {
			__ensure(_recursion);
			if(--_recursion)
				return;
		}

		// Reset the mutex to the unlocked state.
		auto state = __atomic_exchange_n(&_state, 0, __ATOMIC_RELEASE);
		__ensure((state & ownerMask) == mlibc::this_tid());

		if(state & waitersBit) {
			// Wake the futex if there were waiters. Since the mutex might not exist at this location
			// anymore, we must conservatively ignore EACCES and EINVAL which may occur as a result.
			int e = mlibc::sys_futex_wake((int *)&_state);
			__ensure(e >= 0 || e == EACCES || e == EINVAL);
		}
	}
private:
	uint32_t _state;
	uint32_t _recursion;
};

using FutexLock = FutexLockImpl<false>;
using RecursiveFutexLock = FutexLockImpl<true>;

#endif
