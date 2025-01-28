
// for memcpy()
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>

#include <frg/eternal.hpp>
#include <frg/vector.hpp>

struct ExitHandler {
	void (*function)(void *);
	void *argument;
	void *dsoHandle;
};

using ExitQueue = frg::vector<ExitHandler, MemoryAllocator>;

ExitQueue &getExitQueue() {
	// use frg::eternal to prevent the compiler from scheduling the destructor
	// by generating a call to __cxa_atexit().
	static frg::eternal<ExitQueue> singleton(getAllocator());
	return singleton.get();
}

extern "C" int __cxa_atexit(void (*function)(void *), void *argument, void *handle) {
	ExitHandler handler;
	handler.function = function;
	handler.argument = argument;
	handler.dsoHandle = handle;
	getExitQueue().push(handler);
	return 0;
}

extern "C" void __dlapi_exit();

extern "C" void __cxa_finalize(void *dso) {
	ExitQueue &eq = getExitQueue();
	for (size_t i = eq.size(); i > 0; i--) {
		auto &handler = eq[i - 1];
		if (!handler.function)
			continue;

		if (!dso || handler.dsoHandle == dso) {
			handler.function(handler.argument);
			handler.function = nullptr;
		}
	}
}

// In static builds, these should be provided by the crtbegin.o/crtend.o that
// is linked into the executable.
#ifndef MLIBC_STATIC_BUILD
// This is referenced by the compiler when generating constructors for global
// C++ objects so that it can call __cxa_finalize with a unique argument.
extern "C" { [[gnu::visibility("hidden")]] void *__dso_handle; }

[[gnu::destructor]] void __mlibc_do_destructors() {
	// In normal programs this call to __cxa_finalize is provided by libgcc.
	__cxa_finalize(&__dso_handle);
}
#endif

void __mlibc_do_finalize() {
	// Invoke any handlers registered with atexit (NOT associated with a DSO).
	// Note that we deliberately do not invoke other handlers here, since
	// that would destroy mlibc's global objects including stdout and flushing
	// open FILEs, but we'd like those to be available to [[gnu::destructor]]
	// functions which we invoke below.
	ExitQueue &eq = getExitQueue();
	for (size_t i = eq.size(); i > 0; i--) {
		auto &handler = eq[i - 1];
		if (!handler.function)
			continue;

		if (!handler.dsoHandle) {
			handler.function(handler.argument);
			handler.function = nullptr;
		}
	}

	// Call fini/fini_array functions of each loaded object. This is necessary
	// to implement [[gnu::destructor]]. Note that C++ applications will call
	// __cxa_finalize from here.
	__dlapi_exit();
}
