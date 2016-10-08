
#include <stdlib.h>
#include <sys/auxv.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <xuniverse.frigg_pb.hpp>

// defined by the POSIX library
void __mlibc_initLocale();
// defined by the POSIX library
void __mlibc_initStdio();
// defined in malloc.cpp
void __mlibc_initMalloc();
// defined in file.cpp
void __mlibc_initFs();

// declared in posix-pipe.hpp
frigg::LazyInitializer<helx::EventHub> eventHub;
frigg::LazyInitializer<helx::Pipe> posixPipe;
frigg::LazyInitializer<helx::Pipe> fsPipe;

// declared in posix-pipe.hpp
int64_t allocPosixRequest() {
	static int64_t next = 1;
	return next++;
}

// TODO: this function needs to be removed after we fix fork() semantics.
void __mlibc_reinitPosixPipe() {
	// we have to discard these objects here as they might already
	// be initialized after a fork()
	eventHub.discard();
	posixPipe.discard();
	
	eventHub.initialize(helx::EventHub::create());

	unsigned long fs_server;
	if(peekauxval(AT_FS_SERVER, &fs_server))
		__ensure(!"No AT_FS_SERVER specified");

	fsPipe.initialize(fs_server);
	
	// TODO: connect to the POSIX server if the profile allows it.
/*	const char *posix_path = "local/posix";
	HelHandle posix_handle;
	HEL_CHECK(helRdOpen(posix_path, strlen(posix_path), &posix_handle));
	
	int64_t async_id;
	HEL_CHECK(helSubmitConnect(posix_handle, eventHub->getHandle(), 0, 0, &async_id));
	HEL_CHECK(helCloseDescriptor(posix_handle));
	
	helx::Pipe pipe;
	HelError connect_error;
	eventHub->waitForConnect(async_id, connect_error, pipe);
	HEL_CHECK(connect_error);
	posixPipe.initialize(frigg::move(pipe));*/
}

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();
	__mlibc_initMalloc();
	__mlibc_initFs();
	__mlibc_initStdio();

	__mlibc_reinitPosixPipe();
}

// __dso_handle is usually defined in crtbeginS.o
// Since we link with -nostdlib we have to manually define it here
__attribute__ (( visibility("hidden") )) void *__dso_handle;

extern "C" int main(int argc, char *argv[], char *env[]);

// not declared in any header
extern char **environ;

extern "C" void __mlibc_entry() {
	char *empty_argv[] = { nullptr };

	int result = main(1, empty_argv, environ);
	exit(result);
}

