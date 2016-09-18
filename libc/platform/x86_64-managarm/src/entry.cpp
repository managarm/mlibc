
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

// declared in posix-pipe.hpp
frigg::LazyInitializer<helx::EventHub> eventHub;
frigg::LazyInitializer<helx::Pipe> posixPipe;

// declared in posix-pipe.hpp
int64_t allocPosixRequest() {
	static int64_t next = 1;
	return next++;
}

void __mlibc_reinitPosixPipe() {
	// we have to discard these objects here as they might already
	// be initialized after a fork()
	eventHub.discard();
	posixPipe.discard();
	
	eventHub.initialize(helx::EventHub::create());
	
	helx::Pipe superior(kHelThisUniverse);

	// determine the profile we are running in
	{
		managarm::xuniverse::CntRequest<MemoryAllocator> request(*memoryAllocator);
		request.set_req_type(managarm::xuniverse::CntReqType::GET_PROFILE);

		frigg::String<MemoryAllocator> serialized(*memoryAllocator);
		request.SerializeToString(&serialized);

		HelError error;
		superior.sendStringReqSync(serialized.data(), serialized.size(), *eventHub,
				0, 0, error);
		HEL_CHECK(error);
	}
	{
		uint8_t buffer[128];
		HelError error;
		size_t length;
		superior.recvStringRespSync(buffer, 128, *eventHub,
				0, 0, error, length);
		HEL_CHECK(error);
	}

	superior.release();
	
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

