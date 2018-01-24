
// for memcpy()
#include <string.h>

#include <bits/ensure.h>

#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>

#include <frigg/initializer.hpp>
#include <frigg/vector.hpp>

struct ExitHandler {
	void (*function)(void *);
	void *argument;
	void *dsoHandle;
};

using ExitQueue = frigg::Vector<ExitHandler, MemoryAllocator>;

ExitQueue &getExitQueue() {
	// use frigg::Eternal to prevent the compiler from scheduling the destructor
	// by generating a call to __cxa_atexit().
	static frigg::Eternal<ExitQueue> singleton(getAllocator());
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

