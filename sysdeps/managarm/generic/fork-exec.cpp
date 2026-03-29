
// for _Exit()
#include <bits/errors.hpp>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

// for fork() and execve()
#include <unistd.h>
// for sched_yield()
#include <sched.h>
#include <stdio.h>
// for getrusage()
#include <sys/resource.h>
// for waitpid()
#include <pthread.h>
#include <sys/wait.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/thread-entry.hpp>
#include <posix.frigg_bragi.hpp>
#include <protocols/posix/supercalls.hpp>

namespace mlibc {

pid_t Sysdeps<FutexTid>::operator()() {
	HelWord tid = 0;
	HEL_CHECK(helSyscall0_1(kHelCallSuper + posix::superGetTid, &tid));

	return tid;
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	// This implementation is inherently signal-safe.
	int err = 0;

	if (time) {
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));

		err = helFutexWait(pointer, expected, tick + time->tv_nsec + time->tv_sec * 1000000000);
	} else {
		err = helFutexWait(pointer, expected, -1);
	}

	switch (err) {
		case kHelErrNone: return 0;
		case kHelErrTimeout: return ETIMEDOUT;
		case kHelErrCancelled: return EINTR;
		case kHelErrIllegalArgs: return EINVAL;
		case kHelErrFutexRace: return EAGAIN;
		default: {
			mlibc::infoLogger() << "mlibc: helFutexWait returned unexpected error "
								<< err << frg::endlog;
			return EINVAL;
		}
	}
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	// This implementation is inherently signal-safe.
	if (helFutexWake(pointer, all ? UINT32_MAX : 1))
		return -1;
	return 0;
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	SignalGuard sguard;
	mlibc::thread_testcancel();

	if (ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}

	managarm::posix::CntRequest<SysdepsAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::WAIT);
	req.set_pid(pid);
	req.set_flags(flags);
	req.set_cancellation_id(allocateCancellationId());

	auto [offer, send_head, recv_resp] = exchangeMsgsSyncCancellable(
	    getPosixLane(),
	    req.cancellation_id(),
	    -1,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*ret_pid = resp.pid();
	if (*ret_pid == 0)
		return 0;

	if (status)
		*status = resp.mode();

	if (ru != nullptr) {
		ru->ru_utime.tv_sec = resp.ru_user_time() / 1'000'000'000;
		ru->ru_utime.tv_usec = (resp.ru_user_time() % 1'000'000'000) / 1'000;
	}

	return 0;
}

int Sysdeps<Waitid>::operator()(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	SignalGuard sguard;

	mlibc::thread_testcancel();

	managarm::posix::WaitIdRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_idtype(idtype);
	req.set_id(id);
	req.set_flags(options);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::WaitIdResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	info->si_pid = resp.pid();
	info->si_uid = resp.uid();
	info->si_code = resp.sig_code();
	switch (info->si_code) {
		case CLD_EXITED:
			info->si_status = WEXITSTATUS(resp.sig_status());
			break;
		case CLD_KILLED:
		case CLD_DUMPED:
		case CLD_STOPPED:
			info->si_signo = WSTOPSIG(resp.sig_status());
			break;
		case CLD_CONTINUED:
			info->si_signo = SIGCHLD;
			break;
	}
	return 0;
}

void Sysdeps<Exit>::operator()(int status) {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + posix::superExit, status));
	__builtin_trap();
}

void Sysdeps<Yield>::operator()() {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helYield());
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	SignalGuard sguard;
	// TODO: this only handles cancellations up to this point; the syscall does not get cancelled
	mlibc::thread_testcancel();

	globalQueue.trim();

	uint64_t now;
	HEL_CHECK(helGetClock(&now));

	HelSqAwaitClock sqData{};
	sqData.counter = now + uint64_t(*secs) * 1000000000 + uint64_t(*nanos);
	frg::array<frg::span<const std::byte>, 1> segments{
	    frg::span<const std::byte>{reinterpret_cast<const std::byte *>(&sqData), sizeof(sqData)}
	};
	globalQueue.pushSq(kHelSubmitAwaitClock, 0, segments);

	auto element = globalQueue.dequeueSingle();
	auto result = parseSimple(element);
	HEL_CHECK(result->error);

	*secs = 0;
	*nanos = 0;

	return 0;
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	// This implementation is inherently signal-safe.
	int res;

	sigset_t full_sigset;
	res = sigfillset(&full_sigset);
	__ensure(!res);

	sigset_t former_sigset;
	res = sigprocmask(SIG_SETMASK, &full_sigset, &former_sigset);
	__ensure(!res);

	HelWord out;
	HEL_CHECK(helSyscall0_1(kHelCallSuper + posix::superFork, &out));
	*child = out;

	if (!out) {
		clearCachedInfos();
		globalQueue.recreateQueue();
	}

	res = sigprocmask(SIG_SETMASK, &former_sigset, nullptr);
	__ensure(!res);

	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	// TODO: Make this function signal-safe!
	SignalGuard sguard;

	frg::string<SysdepsAllocator> args_area(getSysdepsAllocator());
	for (auto it = argv; *it; ++it)
		args_area += frg::string_view{*it, strlen(*it) + 1};

	frg::string<SysdepsAllocator> env_area(getSysdepsAllocator());
	for (auto it = envp; *it; ++it)
		env_area += frg::string_view{*it, strlen(*it) + 1};

	uintptr_t out;

	HEL_CHECK(helSyscall6_1(
	    kHelCallSuper + posix::superExecve,
	    reinterpret_cast<uintptr_t>(path),
	    strlen(path),
	    reinterpret_cast<uintptr_t>(args_area.data()),
	    args_area.size(),
	    reinterpret_cast<uintptr_t>(env_area.data()),
	    env_area.size(),
	    &out
	));

	return out;
}

gid_t Sysdeps<GetGid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetGidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int Sysdeps<SetGid>::operator()(gid_t gid) {
	SignalGuard sguard;

	managarm::posix::SetGidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_uid(gid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

gid_t Sysdeps<GetEgid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetEgidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int Sysdeps<SetEgid>::operator()(gid_t egid) {
	SignalGuard sguard;

	managarm::posix::SetEgidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_uid(egid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int Sysdeps<SetResgid>::operator()(gid_t rgid, gid_t egid, gid_t sgid) {
	// TODO: handle saved set-user-ID
	(void)sgid;

	int real = sysdep<SetGid>(rgid);
	if (real)
		return real;

	int effective = sysdep<SetEgid>(egid);
	if (effective)
		return effective;

	return 0;
}

uid_t Sysdeps<GetUid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetUidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int Sysdeps<SetUid>::operator()(uid_t uid) {
	SignalGuard sguard;

	managarm::posix::SetUidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_uid(uid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

uid_t Sysdeps<GetEuid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetEuidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.uid();
}

int Sysdeps<SetEuid>::operator()(uid_t euid) {
	SignalGuard sguard;

	managarm::posix::SetEuidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_uid(euid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int Sysdeps<SetResuid>::operator()(uid_t ruid, uid_t euid, uid_t suid) {
	// TODO: handle saved set-user-ID
	(void)suid;

	int real = sysdep<SetUid>(ruid);
	if (real)
		return real;

	int effective = sysdep<SetEuid>(euid);
	if (effective)
		return effective;

	return 0;
}

int Sysdeps<SetReuid>::operator()(uid_t ruid, uid_t euid) {
	int real = sysdep<SetUid>(ruid);
	if (real)
		return real;

	int effective = sysdep<SetEuid>(euid);
	if (effective)
		return effective;

	return 0;
}

int Sysdeps<SetRegid>::operator()(gid_t rgid, gid_t egid) {
	int real = sysdep<SetGid>(rgid);
	if (real)
		return real;

	int effective = sysdep<SetEgid>(egid);
	if (effective)
		return effective;

	return 0;
}

pid_t Sysdeps<GetTid>::operator()() {
	HelWord tid = 0;
	HEL_CHECK(helSyscall0_1(kHelCallSuper + posix::superGetTid, &tid));

	return tid;
}

pid_t Sysdeps<GetPid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetPidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

pid_t Sysdeps<GetPpid>::operator()() {
	SignalGuard sguard;

	managarm::posix::GetPpidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid) {
	SignalGuard sguard;

	managarm::posix::GetSidRequest<SysdepsAllocator> req(getSysdepsAllocator());
	req.set_pid(pid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS) {
		return resp.error() | toErrno;
	}

	*sid = resp.pid();
	return 0;
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	SignalGuard sguard;

	managarm::posix::GetPgidRequest<SysdepsAllocator> req(getSysdepsAllocator());
	req.set_pid(pid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS) {
		return resp.error() | toErrno;
	}

	*pgid = resp.pid();
	return 0;
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	SignalGuard sguard;

	managarm::posix::SetPgidRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_pid(pid);
	req.set_pgid(pgid);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int Sysdeps<GetRusage>::operator()(int scope, struct rusage *usage) {
	memset(usage, 0, sizeof(struct rusage));

	SignalGuard sguard;

	managarm::posix::CntRequest<SysdepsAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::GET_RESOURCE_USAGE);
	req.set_mode(scope);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	usage->ru_utime.tv_sec = resp.ru_user_time() / 1'000'000'000;
	usage->ru_utime.tv_usec = (resp.ru_user_time() % 1'000'000'000) / 1'000;

	return 0;
}

int Sysdeps<GetSchedparam>::operator()(void *tcb, int *policy, struct sched_param *param) {
	if (tcb != mlibc::get_current_tcb()) {
		return ESRCH;
	}

	*policy = SCHED_OTHER;
	int prio = 0;
	// TODO(no92): use helGetPriority(kHelThisThread) here
	mlibc::infoLogger() << "\e[31mlibc: sys_getschedparam always returns priority 0\e[39m"
	                    << frg::endlog;
	param->sched_priority = prio;

	return 0;
}

int Sysdeps<SetSchedparam>::operator()(void *tcb, int policy, const struct sched_param *param) {
	if (tcb != mlibc::get_current_tcb()) {
		return ESRCH;
	}

	if (policy != SCHED_OTHER) {
		return EINVAL;
	}

	HEL_CHECK(helSetPriority(kHelThisThread, param->sched_priority));

	return 0;
}

int Sysdeps<Clone>::operator()(void *tcb, pid_t *tid_out, void *stack) {
	(void)tcb;

	HelWord posixErr = 0;
	HelWord tid = 0;
	posix::superCloneArgs args{
	    .flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD),
	};

	HEL_CHECK(helSyscall3_2(
	    kHelCallSuper + posix::superClone,
	    reinterpret_cast<HelWord>(__mlibc_start_thread),
	    reinterpret_cast<HelWord>(stack),
	    reinterpret_cast<HelWord>(&args),
	    &posixErr,
	    &tid
	));

	if (posixErr)
		return managarm::posix::Errors(posixErr) | toErrno;

	if (tid_out)
		*tid_out = tid;

	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
#if defined(__x86_64__)
	HEL_CHECK(helWriteFsBase(pointer));
#elif defined(__aarch64__)
	uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
	addr += sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(addr));
#elif defined(__riscv) && __riscv_xlen == 64
	uintptr_t tp = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" : : "r"(tp) : "memory");
#else
#error Unknown architecture
#endif
	return 0;
}

void Sysdeps<ThreadExit>::operator()() {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + posix::superThreadExit, 0));
	__builtin_trap();
}

int Sysdeps<ThreadSetname>::operator()(void *tcb, const char *name) {
	if (strlen(name) > 15) {
		return ERANGE;
	}

	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;

	if (asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if (int e = sysdep<Open>(path, O_WRONLY, 0, &fd); e) {
		return e;
	}

	if (int e = sysdep<Write>(fd, name, strlen(name) + 1, nullptr)) {
		return e;
	}

	sysdep<Close>(fd);

	pthread_setcancelstate(cs, nullptr);

	return 0;
}

int Sysdeps<ThreadGetname>::operator()(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;
	ssize_t real_size = 0;

	if (asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if (int e = sysdep<Open>(path, O_RDONLY | O_CLOEXEC, 0, &fd); e) {
		return e;
	}

	if (int e = sysdep<Read>(fd, name, size, &real_size)) {
		return e;
	}

	name[real_size - 1] = 0;
	sysdep<Close>(fd);

	pthread_setcancelstate(cs, nullptr);

	if (static_cast<ssize_t>(size) <= real_size) {
		return ERANGE;
	}

	return 0;
}

} // namespace mlibc
