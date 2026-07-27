#pragma once

#include <abi-bits/clockid_t.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/sched_param.h>
#include <atomic>
#include <bits/cpu_set.h>
#include <bits/sigset_t.h>
#include <bits/size_t.h>
#include <bits/types.h>
#include <bits/pthread_types.h>
#include <new>
#include <threads.h>
#include <time.h>

static constexpr size_t default_stacksize = 0x200000;
static constexpr size_t default_guardsize = 4096;

struct __mlibc_threadattr {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, pthread_attr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_threadattr, __mlibc_threadattr>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	size_t __guardsize = default_guardsize;

	void *__stackaddr = nullptr;
	size_t __stacksize = default_stacksize;

	/* __detachstate: 0 if PTHREAD_CREATE_JOINABLE */
	__mlibc_uint32 __detachstate: 1 = 0;
	/* __inheritsched: 0 if PTHREAD_INHERIT_SCHED */
	__mlibc_uint32 __inheritsched: 1 = 0;
	/* __scope: 0 if PTHREAD_SCOPE_SYSTEM */
	__mlibc_uint32 __scope: 1 = 0;
	__mlibc_uint32 __unused_flags: 29;

	struct sched_param __schedparam{};
	int __schedpolicy = SCHED_OTHER;

	cpu_set_t *__cpuset = nullptr;
	size_t __cpusetsize = 0;

	sigset_t __sigmask;
	int __sigmaskset = 0;
};

struct __mlibc_mutex {
	static constexpr unsigned int mutexRecursive = 1;
	static constexpr unsigned int mutexErrorCheck = 2;
	static constexpr unsigned int mutexShared = 4;

	// TODO: either use uint32_t or determine the bit based on sizeof(int).
	static constexpr unsigned int mutex_owner_mask = (static_cast<__mlibc_uint32>(1) << 30) - 1;
	static constexpr unsigned int mutex_waiters_bit = static_cast<__mlibc_uint32>(1) << 31;

	static __mlibc_mutex *from(pthread_mutex_t *mutex) {
		return std::launder(reinterpret_cast<__mlibc_mutex *>(mutex));
	}

	static __mlibc_mutex *from(mtx_t *mutex) {
		return std::launder(reinterpret_cast<__mlibc_mutex *>(mutex));
	}

	unsigned int __mlibc_state = 0;
	unsigned int __mlibc_recursion = 0;
	unsigned int __mlibc_flags = 0;
	int __mlibc_prioceiling = 0;

	char __reserved[__MLIBC_THREAD_MUTEX_SIZE - (sizeof(int) * 4)] = { 0 };
} __attribute__((aligned(__INTPTR_WIDTH__ / 8)));

struct __mlibc_mutexattr {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, pthread_mutexattr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_mutexattr, __mlibc_mutexattr>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	int __mlibc_type = __MLIBC_THREAD_MUTEX_DEFAULT;
	int __mlibc_robust = __MLIBC_THREAD_MUTEX_STALLED;
	int __mlibc_protocol = __MLIBC_THREAD_PRIO_NONE;
	int __mlibc_pshared = __MLIBC_THREAD_PROCESS_PRIVATE;
	int __mlibc_prioceiling = 0;
};

struct __mlibc_cond {
	static __mlibc_cond *from(pthread_cond_t *cond) {
		return std::launder(reinterpret_cast<__mlibc_cond *>(cond));
	}

	static __mlibc_cond *from(cnd_t *cond) {
		return std::launder(reinterpret_cast<__mlibc_cond *>(cond));
	}

	std::atomic<__mlibc_uint32> __mlibc_seq = 0;
	__mlibc_uint32 __mlibc_flags = __MLIBC_THREAD_PROCESS_PRIVATE;
	clockid_t __mlibc_clock = CLOCK_REALTIME;

	char __reserved[__MLIBC_THREAD_COND_SIZE - (sizeof(__mlibc_uint32) * 2 + sizeof(clockid_t))] = {0};
} __attribute__((aligned(__INTPTR_WIDTH__ / 8)));

struct __mlibc_condattr {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, pthread_condattr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_condattr, __mlibc_condattr>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	int __mlibc_pshared = __MLIBC_THREAD_PROCESS_PRIVATE;
	clockid_t __mlibc_clock = CLOCK_REALTIME;
};

struct __mlibc_barrier {
	__mlibc_barrier() = delete;
	__mlibc_barrier(unsigned int count, bool pshared)
	: __mlibc_count{count},
	  __mlibc_flags{static_cast<unsigned int>(
	      pshared ? __MLIBC_THREAD_PROCESS_SHARED : __MLIBC_THREAD_PROCESS_PRIVATE
	  )} {}

	static __mlibc_barrier *from(pthread_barrier_t *barrier) {
		return std::launder(reinterpret_cast<__mlibc_barrier *>(barrier));
	}

	std::atomic<unsigned int> __mlibc_waiting = 0;
	std::atomic<unsigned int> __mlibc_inside = 0;
	unsigned int __mlibc_count;
	std::atomic<unsigned int> __mlibc_seq = 0;
	unsigned int __mlibc_flags;

	char __reserved[__MLIBC_THREAD_BARRIER_SIZE - (sizeof(unsigned int) * 5)] = { 0 };
} __attribute__((aligned(__INTPTR_WIDTH__ / 8)));

struct __mlibc_barrierattr_struct {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, pthread_barrierattr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_barrierattr_struct, __mlibc_barrierattr_struct>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	int __mlibc_pshared = __MLIBC_THREAD_PROCESS_PRIVATE;
};

struct __mlibc_fair_rwlock {
	__mlibc_fair_rwlock() = delete;
	__mlibc_fair_rwlock(bool pshared)
	: __mlibc_flags{pshared ? __MLIBC_THREAD_PROCESS_SHARED : __MLIBC_THREAD_PROCESS_PRIVATE} {}

	static __mlibc_fair_rwlock *from(pthread_rwlock_t *lock) {
		return std::launder(reinterpret_cast<__mlibc_fair_rwlock *>(lock));
	}

	// TODO: either use uint32_t or determine the bit based on sizeof(int).
	static constexpr unsigned int mutex_owner_mask = (static_cast<uint32_t>(1) << 30) - 1;
	static constexpr unsigned int mutex_waiters_bit = static_cast<uint32_t>(1) << 31;

	// Only valid for the internal __mlibc_m mutex of wrlocks.
	static constexpr unsigned int mutex_excl_bit = static_cast<uint32_t>(1) << 30;

	static constexpr unsigned int rc_count_mask = (static_cast<uint32_t>(1) << 31) - 1;
	static constexpr unsigned int rc_waiters_bit = static_cast<uint32_t>(1) << 31;

	std::atomic<__mlibc_uint32> __mlibc_m = 0; /* Mutex. */
	std::atomic<__mlibc_uint32> __mlibc_rc = 0; /* Reader count (not reference count). */
	int __mlibc_flags;

	char __reserved[__MLIBC_THREAD_RWLOCK_SIZE - (sizeof(__mlibc_uint32) * 2 + sizeof(int))] = { 0 };
} __attribute__((aligned(__INTPTR_WIDTH__ / 8)));

struct __mlibc_rwlockattr {
	template <typename T>
	    requires std::is_same_v<std::remove_cv_t<T>, pthread_rwlockattr_t>
	[[nodiscard]] static auto *from(T *s) noexcept {
		using TargetType =
		    std::conditional_t<std::is_const_v<T>, const __mlibc_rwlockattr, __mlibc_rwlockattr>;

		if (!s)
			return static_cast<TargetType *>(nullptr);

		return std::launder(reinterpret_cast<TargetType *>(s->__heap_ptr));
	}

	int __mlibc_pshared = __MLIBC_THREAD_PROCESS_PRIVATE;
};

struct __mlibc_once {
	static constexpr unsigned int onceComplete = 1;
	static constexpr unsigned int onceLocked = 2;

	static __mlibc_once *from(pthread_once_t *o) {
		return std::launder(reinterpret_cast<__mlibc_once *>(o));
	}

	static __mlibc_once *from(once_flag *o) {
		return std::launder(reinterpret_cast<__mlibc_once *>(o));
	}

	std::atomic<unsigned int> __mlibc_done = 0;
};

struct __mlibc_spinlock {
	static __mlibc_spinlock *from(pthread_spinlock_t *lock) {
		return std::launder(reinterpret_cast<__mlibc_spinlock *>(lock));
	}

	std::atomic<unsigned int> __lock = 0;
};
