
#include <stdlib.h>

#include <mlibc/ensure.h>

void __mlibc_initMalloc();

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	// FIXME: initialize malloc here
	//__mlibc_initMalloc();
}

// __dso_handle is usually defined in crtbeginS.o
// Since we link with -nostdlib we have to manually define it here
__attribute__ (( visibility("hidden") )) int __dso_handle;

extern "C" int main(int argc, char *argv[], char *env[]);

extern "C" void __mlibc_entry() {
	int result = main(0, NULL, NULL);
	exit(result);
}

