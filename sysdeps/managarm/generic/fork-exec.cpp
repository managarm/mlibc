
// for _Exit()
#include <stdlib.h>

#include <string.h>
#include <errno.h>

// for fork() and execve()
#include <unistd.h>
// for sched_yield()
#include <sched.h>
// for getrusage()
#include <sys/resource.h>
// for waitpid()
#include <sys/wait.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/thread-entry.hpp>
#include <mlibc/sysdeps.hpp>
#include <posix.frigg_bragi.hpp>

namespace mlibc {

int sys_futex_tid() {
	SignalGuard sguard;

	managarm::posix::GetTidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, sendHead, recvResp] =
		exchangeMsgsSync(
			getPosixLane(),
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendHead.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

int sys_futex_wait(int *pointer, int expected) {
	// This implementation is inherently signal-safe.
	if(helFutexWait(pointer, expected, -1))
		return -1;
	return 0;
}

int sys_futex_wake(int *pointer) {
	// This implementation is inherently signal-safe.
	if(helFutexWake(pointer))
		return -1;
	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::WAIT);
	req.set_pid(pid);
	req.set_flags(flags);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	if(status)
		*status = resp.mode();
	*ret_pid = resp.pid();
	return 0;
}

void sys_exit(int status) {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + 4, status));
	__builtin_trap();
}

void sys_yield() {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helYield());
}

int sys_sleep(time_t *secs, long *nanos) {
	SignalGuard sguard;
	globalQueue.trim();

	uint64_t now;
	HEL_CHECK(helGetClock(&now));

	uint64_t async_id;
	HEL_CHECK(helSubmitAwaitClock(now + uint64_t(*secs) * 1000000000 + uint64_t(*nanos),
			globalQueue.getQueue(), 0, &async_id));

	auto element = globalQueue.dequeueSingle();
	auto result = parseSimple(element);
	HEL_CHECK(result->error);

	*secs = 0;
	*nanos = 0;

	return 0;
}

int sys_fork(pid_t *child) {
	// This implementation is inherently signal-safe.
	int res;

	sigset_t full_sigset;
	res = sigfillset(&full_sigset);
	__ensure(!res);

	sigset_t former_sigset;
	res = sigprocmask(SIG_SETMASK, &full_sigset, &former_sigset);
	__ensure(!res);

	HelError error;
	asm volatile ("syscall" : "=D"(error), "=S"(*child) : "0"(kHelCallSuper + 2)
			: "rcx", "r11", "rbx", "memory");
	HEL_CHECK(error);

	if(!*child) {
		clearCachedInfos();
		globalQueue.recreateQueue();
	}

	res = sigprocmask(SIG_SETMASK, &former_sigset, nullptr);
	__ensure(!res);

	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	// TODO: Make this function signal-safe!
	frg::string<MemoryAllocator> args_area(getSysdepsAllocator());
	for(auto it = argv; *it; ++it)
		args_area += frg::string_view{*it, strlen(*it) + 1};

	frg::string<MemoryAllocator> env_area(getSysdepsAllocator());
	for(auto it = envp; *it; ++it)
		env_area += frg::string_view{*it, strlen(*it) + 1};

	uintptr_t arg0 = reinterpret_cast<uintptr_t>(path);
	uintptr_t arg1 = strlen(path);
	uintptr_t arg2 = reinterpret_cast<uintptr_t>(args_area.data());
	uintptr_t arg3 = args_area.size();
	uintptr_t arg4 = reinterpret_cast<uintptr_t>(env_area.data());
	uintptr_t arg5 = env_area.size();

	register uintptr_t in0 asm("rsi") = arg0;
	register uintptr_t in1 asm("rdx") = arg1;
	register uintptr_t in2 asm("rax") = arg2;
	register uintptr_t in3 asm("r8") = arg3;
	register uintptr_t in4 asm("r9") = arg4;
	register uintptr_t in5 asm("r10") = arg5;
	asm volatile ("syscall" : "=r"(in0) : "D"(kHelCallSuper + 3), "r"(in0), "r"(in1),
				"r"(in2), "r"(in3), "r"(in4), "r"(in5)
			: "rcx", "r11", "rbx", "memory");
	return in0;
}

gid_t sys_getgid() {
	SignalGuard sguard;

	managarm::posix::GetGidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int sys_setgid(gid_t gid) {
	SignalGuard sguard;

	managarm::posix::SetGidRequest<MemoryAllocator> req(getSysdepsAllocator());

	req.set_uid(gid);

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::ACCESS_DENIED) {
		return EPERM;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

gid_t sys_getegid() {
	SignalGuard sguard;

	managarm::posix::GetEgidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int sys_setegid(gid_t egid) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SET_EGID);
	req.set_uid(egid);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::ACCESS_DENIED) {
		return EPERM;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

uid_t sys_getuid() {
	SignalGuard sguard;

	managarm::posix::GetUidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int sys_setuid(uid_t uid) {
	SignalGuard sguard;

	managarm::posix::SetUidRequest<MemoryAllocator> req(getSysdepsAllocator());

	req.set_uid(uid);

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::ACCESS_DENIED) {
		return EPERM;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

uid_t sys_geteuid() {
	SignalGuard sguard;

	managarm::posix::GetEuidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int sys_seteuid(uid_t euid) {
	SignalGuard sguard;

	managarm::posix::SetEuidRequest<MemoryAllocator> req(getSysdepsAllocator());

	req.set_uid(euid);

	auto [offer, send_head, recv_resp] = 
			exchangeMsgsSync(
					getPosixLane(),
					helix_ng::offer(
							helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::ACCESS_DENIED) {
		return EPERM;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

pid_t sys_getpid() {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::GET_PID);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

pid_t sys_getppid() {
	mlibc::infoLogger() << "mlibc: Broken getppid() called" << frg::endlog;
	return 1;
}

int sys_getrusage(int scope, struct rusage *usage) {
	memset(usage, 0, sizeof(struct rusage));

	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::GET_RESOURCE_USAGE);
	req.set_mode(scope);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);

	usage->ru_utime.tv_sec = resp.ru_user_time() / 1'000'000'000;
	usage->ru_utime.tv_usec = (resp.ru_user_time() % 1'000'000'000) / 1'000;

	return 0;
}

int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *pid_out) {
	HelWord pid = 0;

	void *sp = prepare_stack(entry, user_arg, tcb);

	HEL_CHECK(helSyscall2_1(kHelCallSuper + 9,
				reinterpret_cast<HelWord>(__mlibc_start_thread),
				reinterpret_cast<HelWord>(sp),
				&pid));

	if (pid_out)
		*pid_out = pid;

	return 0;
}

int sys_tcb_set(void *pointer) {
	HEL_CHECK(helWriteFsBase(pointer));
	return 0;
}

void sys_thread_exit() {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + 4, 0));
	__builtin_trap();
}


} //namespace mlibc

