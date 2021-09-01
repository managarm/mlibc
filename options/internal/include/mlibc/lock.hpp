#ifndef MLIBC_LOCK_HPP
#define MLIBC_LOCK_HPP

#include <stdint.h>
#include <mlibc/internal-sysdeps.hpp>
#include <bits/ensure.h>

struct FutexLock {
	FutexLock()
	: _futex{0} { }

	FutexLock(const FutexLock &) = delete;

	FutexLock &operator= (const FutexLock &) = delete;

	static constexpr inline uint32_t waitersBit = (1 << 31);

	void lock() {

		unsigned int expected = 0;
		while(true) {
			if(!expected) {
				// Try to take the mutex here.
				if(__atomic_compare_exchange_n(&_futex, reinterpret_cast<int*>(&expected), 1,
							false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
					return;
				}
			}else{
				if(expected & waitersBit) {
					if(int e = mlibc::sys_futex_wait(&_futex, expected, nullptr); e)
						__ensure(!"sys_futex_wait() failed");

					expected = 0;
				}else{
					unsigned int desired = expected | waitersBit;
					if(__atomic_compare_exchange_n(&_futex, reinterpret_cast<int*>(&expected), desired,
							false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
						expected = desired;
				}
			}
		}
	}

	void unlock() {
		if (__atomic_exchange_n(&_futex, 0, __ATOMIC_RELEASE) & waitersBit)
			if(int e = mlibc::sys_futex_wake(&_futex); e)
				__ensure(!"sys_futex_wake() failed");
	}

private:
	int _futex;
};

#endif
