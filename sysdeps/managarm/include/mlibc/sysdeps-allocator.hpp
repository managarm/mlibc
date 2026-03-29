#pragma once
#include <frg/sharded_slab.hpp>
#include <internal-config.h>

#if defined(__clang__)
#define CAPABILITY(x) __attribute__((capability(x)))
#define SCOPED_CAPABILITY __attribute__((scoped_lockable))
#define CAP_ACQUIRE(...) __attribute__((acquire_capability(__VA_ARGS__)))
#define CAP_RELEASE(...) __attribute__((release_capability(__VA_ARGS__)))
#define CAP_REQUIRES(...) __attribute__((requires_capability(__VA_ARGS__)))
#else
#define CAPABILITY(x)
#define SCOPED_CAPABILITY
#define CAP_ACQUIRE(...)
#define CAP_RELEASE(...)
#define CAP_REQUIRES(...)
#endif

class CAPABILITY("SignalGuardState") SysdepAllocatorCapability{};
extern SysdepAllocatorCapability sysdepAllocatorCapability;


struct ShardedSlabPolicy {
	void *map(size_t size);
	void unmap(void *ptr, size_t size);
};

using SysdepsPool = frg::sharded_slab::pool<ShardedSlabPolicy>;

SysdepsPool &getSysdepsPool();

struct SysdepsAllocator {
	void *allocate(size_t size) { return getSysdepsPool().allocate(size); }
	void free(void *ptr) { getSysdepsPool().deallocate(ptr); }
	void deallocate(void *ptr, size_t) { getSysdepsPool().deallocate(ptr); }
};

extern SysdepsAllocator sysdepsAllocator;

// We need an allocator for message structs in sysdeps functions.
// The "normal" mlibc allocator cannot be used, as the sysdeps function might be called from a signal.
inline SysdepsAllocator &getSysdepsAllocator() CAP_REQUIRES(sysdepAllocatorCapability) {
	return sysdepsAllocator;
}
