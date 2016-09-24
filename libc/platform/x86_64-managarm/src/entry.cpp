
#include <stdlib.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#pragma GCC visibility push(hidden)

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <xuniverse.frigg_pb.hpp>

#pragma GCC visibility pop

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

enum {
	// this value is not part of the ABI
	AT_ILLEGAL = -1,

	AT_NULL = 0,
	AT_PHDR = 3,
	AT_PHENT = 4,
	AT_PHNUM = 5,
	AT_ENTRY = 9,

	AT_OPENFILES = 0x1001,
	AT_FS_SERVER = 0x1102
};

struct Auxiliary {
	int type;
	union {
		long longValue;
		void *pointerValue;
	};
};

struct AuxFileData {
	int fd;
	HelHandle pipe;
};

// FIXME: move this to another file
extern "C" Auxiliary *__rtdl_auxvector();

// TODO: this function needs to be removed after we fix fork() semantics.
void __mlibc_reinitPosixPipe() {
	// we have to discard these objects here as they might already
	// be initialized after a fork()
	eventHub.discard();
	posixPipe.discard();
	
	eventHub.initialize(helx::EventHub::create());

	// parse the auxiliary vector.
	assert(__rtdl_auxvector);
	Auxiliary *element = __rtdl_auxvector();
	while(true) {
		if(element->type == AT_NULL)
			break;

		if(element->type == AT_OPENFILES) {
			__ensure(!"Make this this work correctly");
//			auto data = static_cast<AuxFileData *>(element->pointerValue);
//			fileMap->insert(data->fd, helx::Pipe(data->pipe));
		}else if(element->type == AT_FS_SERVER) {
			fsPipe.initialize(element->longValue);
		}
		element++;
	}
	
	
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

