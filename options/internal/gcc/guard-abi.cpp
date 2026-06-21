
#include <stddef.h>
#include <stdint.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tid.hpp>

extern "C" [[ gnu::visibility("hidden") ]] void __cxa_pure_virtual() {
	mlibc::panicLogger() << "mlibc: Pure virtual function called from IP "
			<< (void *)__builtin_return_address(0) << frg::endlog;
}

static const char __poison_cxa_guard_acquire[]
    __attribute__((used, section(".gnu.warning.__cxa_guard_acquire"))) =
        "mlibc cannot use static singletons, use lazy_eternal instead";

static const char __poison_cxa_guard_release[]
    __attribute__((used, section(".gnu.warning.__cxa_guard_release"))) =
        "mlibc cannot use static singletons, use lazy_eternal instead";
