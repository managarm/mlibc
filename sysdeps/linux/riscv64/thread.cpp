#include <mlibc/thread-entry.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread.hpp>
#include <bits/ensure.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg) {
	// TODO
	asm volatile ("unimp"); // TODO
}

namespace mlibc {

void *prepare_stack(void *entry, void *user_arg) {
	asm volatile ("unimp"); // TODO
	return nullptr; // TODO
}

}
