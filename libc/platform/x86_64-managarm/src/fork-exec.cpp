
// for _Exit()
#include <stdlib.h>

#include <string.h>
#include <errno.h>

// for fork() and execve()
#include <unistd.h>
// for sched_yield()
#include <sched.h>
// for waitpid()
#include <sys/wait.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/debug.hpp>
#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>

uid_t getuid(void) {
	return 0;
}
gid_t getgid(void) {
	return 0;
}

uid_t geteuid(void) {
	return 0;
}
gid_t getegid(void) {
	return 0;
}

pid_t getpid(void) {
	assert(!"Fix this");
/*	managarm::posix::ClientRequest<MemoryAllocator> request(getAllocator());
	request.set_request_type(managarm::posix::ClientRequestType::GET_PID);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(getAllocator());
	request.SerializeToString(&serialized);
	HelError error;
	posixPipe.sendStringReqSync(serialized.data(), serialized.size(),
			eventHub, request_num, 0, error);
	HEL_CHECK(error);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe.recvStringRespSync(buffer, 128, eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(getAllocator());
	response.ParseFromArray(buffer, length);
	assert(response.error() == managarm::posix::Errors::SUCCESS);
	return response.pid();*/
}
pid_t getppid(void) {
	frigg::infoLogger() << "mlibc: Broken getppid() called" << frigg::endLog;
	return 1;
}

pid_t fork(void) {
	HelError error;
	pid_t child;
	asm volatile ("syscall" : "=D"(error), "=S"(child) : "0"(kHelCallSuper + 2));
	HEL_CHECK(error);
	
	if(!child)
		clearCachedInfos();

	return child;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	auto length = strlen(path);
	register uintptr_t r8 asm("r8") = (uintptr_t)path;
	register uintptr_t r9 asm("r9") = length;
	asm volatile ("syscall" : : "D"(kHelCallSuper + 3), "r"(r8), "r"(r9));
	__builtin_trap();
}

pid_t waitpid(pid_t pid, int *status, int flags) {
	frigg::infoLogger() << "mlibc: Broken waitpid("
			<< pid << ", " << flags << ") called!" << frigg::endLog;
	__ensure(flags & WNOHANG);
	errno = ECHILD;
	return -1;
}

void _Exit(int status) {
	HEL_CHECK(helExitThisThread());
	__builtin_unreachable();
}

int sched_yield(void) {
	HEL_CHECK(helYield());
	return 0;
}

