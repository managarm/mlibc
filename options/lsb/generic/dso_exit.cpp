
// for memcpy()
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/cxx-support.hpp>

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

