#include <mlibc/allocator.hpp>

// The cxxabi needs operator delete for *deleting* destructors, i.e., destructors that
// are called by delete expressions. We never use such expressions in mlibc.
// Note that G++ complains if we make the operator hidden,
// thus we use its mangled name as a workaround.

extern "C" [[gnu::visibility("hidden")]] void *_Znwm(size_t size) { // operator new (size_t)
	return getAllocator().allocate(size);
}

extern "C" [[gnu::visibility("hidden")]] void _ZdlPv(void *ptr) { // operator delete (void *)
	getAllocator().free(ptr);
}

extern "C" [[gnu::visibility("hidden")]] void _ZdlPvm(void *ptr, size_t size) { // operator delete (void *, size_t)
	getAllocator().deallocate(ptr, size);
}
