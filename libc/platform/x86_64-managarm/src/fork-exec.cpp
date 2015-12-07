
// for _Exit()
#include <stdlib.h>

#include <string.h>
#include <errno.h>

// for fork() and execve()
#include <unistd.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#pragma GCC visibility push(hidden)

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>

// defined in entry.cpp
void __mlibc_reinitPosixPipe();

// defined in enter-fork.S
extern "C" pid_t __mlibc_enterFork();

extern "C" pid_t __mlibc_doFork(uintptr_t child_ip, uintptr_t child_sp) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::FORK);
	request.set_child_ip(child_ip);
	request.set_child_sp(child_sp);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	assert(response.error() == managarm::posix::Errors::SUCCESS);
	
	return 1;
}

extern "C" void __mlibc_fixForkedChild() {
	__mlibc_reinitPosixPipe();
}

#pragma GCC visibility pop

pid_t fork(void) {
	return __mlibc_enterFork();
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::EXEC);
	request.set_path(frigg::String<MemoryAllocator>(*memoryAllocator, path));

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(), request_num, 0);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::SUCCESS) {
		HEL_CHECK(helExitThisThread());
		__builtin_unreachable();
	}else{
		__ensure(!"Unexpected error in execve()!");
		__builtin_unreachable();
	}
}

void _Exit(int status) {
	HEL_CHECK(helExitThisThread());
	__builtin_unreachable();
}

