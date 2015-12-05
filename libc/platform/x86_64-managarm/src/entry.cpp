
#include <stdlib.h>

#include <mlibc/ensure.h>
#include <mlibc/posix-pipe.hpp>

void __mlibc_initMalloc();

// declared in posix-pipe.hpp
frigg::LazyInitializer<helx::EventHub> eventHub;
frigg::LazyInitializer<helx::Pipe> posixPipe;

// declared in posix-pipe.hpp
int64_t allocPosixRequest() {
	static int64_t next = 1;
	return next++;
}

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	// FIXME: initialize malloc here
	//__mlibc_initMalloc();

	__ensure("wtf");
}

// __dso_handle is usually defined in crtbeginS.o
// Since we link with -nostdlib we have to manually define it here
__attribute__ (( visibility("hidden") )) void *__dso_handle;

extern "C" int main(int argc, char *argv[], char *env[]);

extern "C" void __mlibc_entry() {
	int result = main(0, NULL, NULL);
	exit(result);
}

