
#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>

#include <frigg/initializer.hpp>
#include <frigg/vector.hpp>

struct ExitHandler {
	void (*function)(void *);
	void *argument;
	void *dsoTag;
};

frigg::LazyInitializer<frigg::Vector<ExitHandler, MemoryAllocator>> exitHandlers;

#pragma GCC visibility pop

extern "C" int __cxa_atexit(void (*function)(void *), void *argument, void *dso_tag) {
	// FIXME: initialize this from a global library guard constructor
	__ensure(memoryAllocator);
	if(!exitHandlers)
		exitHandlers.initialize(*memoryAllocator);

	ExitHandler handler;
	handler.function = function;
	handler.argument = argument;
	handler.dsoTag = dso_tag;
	exitHandlers->push(handler);
	return 0;
}

