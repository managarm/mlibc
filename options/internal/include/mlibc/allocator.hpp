#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <mlibc/lock.hpp>
#include <bits/ensure.h>
#include <frg/manual_box.hpp>
#include <frg/slab.hpp>
#include <internal-config.h>

#if !MLIBC_DEBUG_ALLOCATOR

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_pool<VirtualAllocator, FutexLock> MemoryPool;

typedef frg::slab_allocator<VirtualAllocator, FutexLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#else

struct MemoryAllocator {
	void *allocate(size_t size);
	void free(void *ptr);
	void deallocate(void *ptr, size_t size);
	void *reallocate(void *ptr, size_t size);
	size_t get_size(void *ptr);
};

MemoryAllocator &getAllocator();

#endif // !MLIBC_DEBUG_ALLOCATOR

namespace mlibc {

template <typename T>
struct lazy_eternal {
	constexpr lazy_eternal() = default;

	template <typename Self>
	auto &get(this Self &&self) {
		if (__atomic_load_n(&self.initialized_, __ATOMIC_ACQUIRE))
			return *self.box_.get();

		{
			frg::unique_lock lock{self.lock_};
			if (!__atomic_load_n(&self.initialized_, __ATOMIC_RELAXED)) {
				self.box_.initialize();
				__atomic_store_n(&self.initialized_, 1, __ATOMIC_RELEASE);
			}
		}

		return *self.box_.get();
	}

private:
	mutable uint32_t initialized_ = 0;
	mutable FutexLock lock_;
	mutable frg::manual_box<T> box_;
};

} // namespace mlibc

#endif // MLIBC_FRIGG_ALLOC
