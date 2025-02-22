
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

int sys_futex_tid() {
	HelWord tid = 0;
	HEL_CHECK(helSyscall0_1(kHelCallSuper + posix::superGetTid, &tid));

	return tid;
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	// This implementation is inherently signal-safe.
	if (time) {
		if (helFutexWait(pointer, expected, time->tv_nsec + time->tv_sec * 1000000000))
			return -1;
		return 0;
	}
	if (helFutexWait(pointer, expected, -1))
		return -1;
	return 0;
}

int sys_futex_wake(int *pointer) {
	// This implementation is inherently signal-safe.
	if (helFutexWake(pointer))
		return -1;
	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::WAIT);
	req.set_pid(pid);
	req.set_flags(flags);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (status)
		*status = resp.mode();
	*ret_pid = resp.pid();

	if (ru != nullptr) {
		ru->ru_utime.tv_sec = resp.ru_user_time() / 1'000'000'000;
		ru->ru_utime.tv_usec = (resp.ru_user_time() % 1'000'000'000) / 1'000;
	}

	return 0;
}

int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	SignalGuard sguard;

	managarm::posix::WaitIdRequest<MemoryAllocator> req(getSysdepsAllocator());

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

	managarm::posix::WaitIdResponse<MemoryAllocator> resp(getSysdepsAllocator());
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

void sys_exit(int status) {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + posix::superExit, status));
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
	HEL_CHECK(helSubmitAwaitClock(
	    now + uint64_t(*secs) * 1000000000 + uint64_t(*nanos), globalQueue.getQueue(), 0, &async_id
	));

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

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	// TODO: Make this function signal-safe!
	frg::string<MemoryAllocator> args_area(getSysdepsAllocator());
	for (auto it = argv; *it; ++it)
		args_area += frg::string_view{*it, strlen(*it) + 1};

	frg::string<MemoryAllocator> env_area(getSysdepsAllocator());
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

gid_t sys_getgid() {
	SignalGuard sguard;

	managarm::posix::GetGidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
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

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

gid_t sys_getegid() {
	SignalGuard sguard;

	managarm::posix::GetEgidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
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

	managarm::posix::SetEgidRequest<MemoryAllocator> req(getSysdepsAllocator());

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

uid_t sys_getuid() {
	SignalGuard sguard;

	managarm::posix::GetUidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
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

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

uid_t sys_geteuid() {
	SignalGuard sguard;

	managarm::posix::GetEuidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
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

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

pid_t sys_gettid() {
	// TODO: use an actual gettid syscall.
	return sys_getpid();
}

pid_t sys_getpid() {
	SignalGuard sguard;

	managarm::posix::GetPidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

pid_t sys_getppid() {
	SignalGuard sguard;

	managarm::posix::GetPpidRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return resp.pid();
}

int sys_getsid(pid_t pid, pid_t *sid) {
	SignalGuard sguard;

	managarm::posix::GetSidRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() == managarm::posix::Errors::NO_SUCH_RESOURCE) {
		*sid = 0;
		return ESRCH;
	} else if (resp.error() != managarm::posix::Errors::SUCCESS) {
		return resp.error() | toErrno;
	}

	*sid = resp.pid();
	return 0;
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
	SignalGuard sguard;

	managarm::posix::GetPgidRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() == managarm::posix::Errors::NO_SUCH_RESOURCE) {
		*pgid = 0;
		return ESRCH;
	} else if (resp.error() != managarm::posix::Errors::SUCCESS) {
		return resp.error() | toErrno;
	}

	*pgid = resp.pid();
	return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	SignalGuard sguard;

	managarm::posix::SetPgidRequest<MemoryAllocator> req(getSysdepsAllocator());

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_getrusage(int scope, struct rusage *usage) {
	memset(usage, 0, sizeof(struct rusage));

	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	usage->ru_utime.tv_sec = resp.ru_user_time() / 1'000'000'000;
	usage->ru_utime.tv_usec = (resp.ru_user_time() % 1'000'000'000) / 1'000;

	return 0;
}

int sys_getschedparam(void *tcb, int *policy, struct sched_param *param) {
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

int sys_setschedparam(void *tcb, int policy, const struct sched_param *param) {
	if (tcb != mlibc::get_current_tcb()) {
		return ESRCH;
	}

	if (policy != SCHED_OTHER) {
		return EINVAL;
	}

	HEL_CHECK(helSetPriority(kHelThisThread, param->sched_priority));

	return 0;
}

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
	(void)tcb;

	HelWord pid = 0;
	HEL_CHECK(helSyscall2_1(
	    kHelCallSuper + posix::superClone,
	    reinterpret_cast<HelWord>(__mlibc_start_thread),
	    reinterpret_cast<HelWord>(stack),
	    &pid
	));

	if (pid_out)
		*pid_out = pid;

	return 0;
}

int sys_tcb_set(void *pointer) {
#if defined(__aarch64__)
	uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
	addr += sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(addr));
#else
	HEL_CHECK(helWriteFsBase(pointer));
#endif
	return 0;
}

void sys_thread_exit() {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall1(kHelCallSuper + posix::superExit, 0));
	__builtin_trap();
}

int sys_thread_setname(void *tcb, const char *name) {
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
	if (int e = sys_open(path, O_WRONLY, 0, &fd); e) {
		return e;
	}

	if (int e = sys_write(fd, name, strlen(name) + 1, NULL)) {
		return e;
	}

	sys_close(fd);

	pthread_setcancelstate(cs, 0);

	return 0;
}

int sys_thread_getname(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;
	ssize_t real_size = 0;

	if (asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if (int e = sys_open(path, O_RDONLY | O_CLOEXEC, 0, &fd); e) {
		return e;
	}

	if (int e = sys_read(fd, name, size, &real_size)) {
		return e;
	}

	name[real_size - 1] = 0;
	sys_close(fd);

	pthread_setcancelstate(cs, 0);

	if (static_cast<ssize_t>(size) <= real_size) {
		return ERANGE;
	}

	return 0;
}

} // namespace mlibc
