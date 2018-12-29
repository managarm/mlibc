
#include <bits/ensure.h>

// The cxxabi needs operator delete for *deleting* destructors, i.e., destructors that
// are called by delete expressions. We never use such expressions in mlibc.
// Note that G++ complains if we make the operator hidden,
// thus we use it's mangled name as a workaround.
extern "C" [[gnu::visibility("hidden")]] void _ZdlPvm() { // operator delete (void *, size_t)
	__ensure(!"operator delete called! delete expressions cannot be used in mlibc.");
}

