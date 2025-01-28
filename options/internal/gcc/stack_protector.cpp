#include <stdint.h>
#include <string.h>
#include <mlibc/debug.hpp>
#include <mlibc/stack_protector.hpp>

uintptr_t __stack_chk_guard = 0;

namespace mlibc {

void initStackGuard(void *entropy) {
	if(entropy != nullptr) {
		memcpy(&__stack_chk_guard, entropy, sizeof(__stack_chk_guard));
	} else {
		// If no entropy is available, set it to the terminator canary
		__stack_chk_guard = 0;
		__stack_chk_guard |= ('\n' << 16);
		__stack_chk_guard |= (255 << 24);
	}
}

} // namespace mlibc

extern "C" [[noreturn]] void __stack_chk_fail() {
	mlibc::panicLogger() << "Stack smashing detected!" << frg::endlog;
	__builtin_unreachable();
}

extern "C" [[noreturn, gnu::visibility("hidden")]] void __stack_chk_fail_local() {
	__stack_chk_fail();
};

