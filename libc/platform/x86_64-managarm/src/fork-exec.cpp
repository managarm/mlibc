
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

unsigned int sleep(unsigned int secs) {
	globalQueue.trim();

	uint64_t now;
	HEL_CHECK(helGetClock(&now));

	HEL_CHECK(helSubmitAwaitClock(now + uint64_t{secs} * 1000000000,
			globalQueue.getQueue(), 0));

	auto element = globalQueue.dequeueSingle();
	auto result = parseSimple(element);
	HEL_CHECK(result->error);

	return 0;
}

// In contrast to sleep() this functions returns 0/-1 on success/failure.
int usleep(useconds_t usecs) {
	globalQueue.trim();

	uint64_t now;
	HEL_CHECK(helGetClock(&now));

	HEL_CHECK(helSubmitAwaitClock(now + uint64_t{usecs} * 1000,
			globalQueue.getQueue(), 0));

	auto element = globalQueue.dequeueSingle();
	auto result = parseSimple(element);
	HEL_CHECK(result->error);

	return 0;
}

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
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::GET_PID);

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
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
	asm volatile ("syscall" : : "D"(kHelCallSuper + 4));
	__builtin_trap();
}

int sched_yield(void) {
	HEL_CHECK(helYield());
	return 0;
}

