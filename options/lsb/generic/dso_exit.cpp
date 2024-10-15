
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
	if(!dso) {
		for(size_t i = eq.size(); i > 0; i--) {
			auto handler = &eq[i - 1];
			if(!handler->function)
				continue;

			handler->function(handler->argument);
			handler->function = nullptr;
		}
	}else {
		for(size_t i = eq.size(); i > 0; i--) {
			auto handler = &eq[i - 1];
			if(handler->dsoHandle != dso || !handler->function)
				continue;

			handler->function(handler->argument);
			handler->function = nullptr;
		}
	}
}

void __mlibc_do_finalize() {
	ExitQueue &eq = getExitQueue();
	for(size_t i = eq.size(); i > 0; i--) {
		auto handler = &eq[i - 1];
		if(handler->dsoHandle || !handler->function)
			continue;
		handler->function(handler->argument);
		handler->function = nullptr;
	}

	__dlapi_exit();
}
