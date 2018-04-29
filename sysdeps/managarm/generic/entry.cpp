
#include <pthread.h>
#include <stdlib.h>
#include <sys/auxv.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/cxx-support.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

// defined by the POSIX library
void __mlibc_initLocale();
// defined by the POSIX library
void __mlibc_initStdio();

extern "C" uintptr_t *__dlapi_entrystack();

// declared in posix-pipe.hpp
thread_local Queue globalQueue;

void *__mlibc_clk_tracker_page;

namespace {
	thread_local HelHandle *cachedFileTable;
	
	// This construction is a bit weird: Even though the variables above
	// are thread_local we still protect their initialization with a pthread_once_t
	// (instead of using a C++ constructor).
	// We do this in order to able to clear the pthread_once_t after a fork.
	pthread_once_t hasCachedInfos = PTHREAD_ONCE_INIT;

	void actuallyCacheInfos() {
		HelError error;
		asm volatile ("syscall" : "=D"(error), "=c"(__mlibc_clk_tracker_page),
				"=S"(cachedFileTable) : "0"(kHelCallSuper + 1));
		HEL_CHECK(error);
	}
}

HelHandle *cacheFileTable() {
	pthread_once(&hasCachedInfos, &actuallyCacheInfos);
	return cachedFileTable;
}

void clearCachedInfos() {
	hasCachedInfos = PTHREAD_ONCE_INIT;
}

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

static int __mlibc_argc;
static char **__mlibc_argv;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();
	__mlibc_initStdio();

	// Parse the environment.
	// TODO: Copy the arguments instead of pointing to them?
	auto env = __dlapi_entrystack();
	__mlibc_argc = *env++;
	__mlibc_argv = reinterpret_cast<char **>(env);
	env += __mlibc_argc; // Skip all arguments.
	__ensure(!*env);
	env++;

	while(*env) {
		auto string = reinterpret_cast<char *>(*env);
		auto fail = putenv(string);
		__ensure(!fail);
		env++;
	}
}

// The environmet was build by the LibraryGuard.
extern char **environ;

extern "C" void __mlibc_entry(int (*main_function)(int argc, char *argv[], char *env[])) {
	auto result = main_function(__mlibc_argc, __mlibc_argv, environ);
	exit(result);
}

