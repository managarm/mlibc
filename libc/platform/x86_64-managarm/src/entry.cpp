
#include <pthread.h>
#include <stdlib.h>
#include <sys/auxv.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

// defined by the POSIX library
void __mlibc_initLocale();
// defined by the POSIX library
void __mlibc_initStdio();

// declared in posix-pipe.hpp
thread_local Queue globalQueue;

namespace {
	thread_local HelHandle *cachedFileTable;
	
	// This construction is a bit weird: Even though the variables above
	// are thread_local we still protect their initialization with a pthread_once_t.
	// We do this in order to able to clear them after a fork.
	pthread_once_t hasCachedInfos = PTHREAD_ONCE_INIT;

	void initCachedInfos() {
		frigg::infoLogger() << "initCachedInfos()" << frigg::endLog;
		struct FileEntry {
			int fd;
			HelHandle pipe;
		};

		cachedFileTable = (HelHandle *)malloc(sizeof(HelHandle) * 1024);
		memset(cachedFileTable, 0, sizeof(HelHandle) * 1024);

		unsigned long openfiles;
		if(!peekauxval(AT_OPENFILES, &openfiles)) {
			for(auto entry = (FileEntry *)openfiles; entry->fd != -1; ++entry)
				cachedFileTable[entry->fd] = entry->pipe;
		}
	}

	void actuallyCacheInfos() {
		HelError error;
		asm volatile ("syscall" : "=D"(error), "=S"(cachedFileTable) : "0"(kHelCallSuper + 1));
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

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();
	__mlibc_initStdio();

	pthread_once(&hasCachedInfos, &initCachedInfos);
}

extern "C" int main(int argc, char *argv[], char *env[]);

// not declared in any header
extern char **environ;

extern "C" void __mlibc_entry() {
	char *empty_argv[] = { nullptr };

	int result = main(1, empty_argv, environ);
	exit(result);
}

