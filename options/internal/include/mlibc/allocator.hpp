#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <mlibc/lock.hpp>
#include <bits/ensure.h>
#include <frg/manual_box.hpp>
#include <frg/slab.hpp>
#include <internal-config.h>

#ifdef MLIBC_BUILDING_RTLD

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_pool<VirtualAllocator, FutexLock> LdsoPool;

typedef frg::slab_allocator<VirtualAllocator, FutexLock> LdsoAllocator;

LdsoAllocator &getLdsoAllocator();

#else // MLIBC_BUILDING_RTLD

#include <frg/sharded_slab.hpp>

// map()/unmap() shared by the sharded-slab policies below.
struct ShardedSlabBasePolicy {
	void *map(size_t size);
	void unmap(void *ptr, size_t size);
};

// Policy for the main allocator.
struct ShardedSlabPolicy : ShardedSlabBasePolicy {
	using mutex_type = FutexLock;

	static constexpr bool support_overaligned = true;
};

#if !MLIBC_DEBUG_ALLOCATOR

using MemoryDomain = frg::sharded_slab::domain<ShardedSlabPolicy>;
using MemoryPool = frg::sharded_slab::pool<ShardedSlabPolicy>;

MemoryPool &getMemoryPool();

struct MemoryAllocator {
	void *allocate(size_t size) { return getMemoryPool().allocate(size); }
	void *allocate(size_t size, size_t alignment) { return getMemoryPool().allocate(size, alignment); }
	void free(void *ptr) { getMemoryPool().deallocate(ptr); }
	void deallocate(void *ptr, size_t) { getMemoryPool().deallocate(ptr); }
	void *reallocate(void *ptr, size_t size) { return getMemoryPool().reallocate(ptr, size); }
	size_t get_size(void *ptr) { return getMemoryPool().get_size(ptr); }
};

MemoryAllocator &getAllocator();

#else

struct MemoryAllocator {
	void *allocate(size_t size);
	void *allocate(size_t size, size_t alignment);
	void free(void *ptr);
	void deallocate(void *ptr, size_t size);
	void *reallocate(void *ptr, size_t size);
	size_t get_size(void *ptr);
};

MemoryAllocator &getAllocator();

#endif // !MLIBC_DEBUG_ALLOCATOR

#endif // MLIBC_BUILDING_RTLD

namespace mlibc {

template <typename T>
struct lazy_eternal {
	constexpr lazy_eternal() = default;

	template <typename Self>
	auto &get(this Self &self) {
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
