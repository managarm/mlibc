#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hel-syscalls.h>
#include <hel.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <posix.frigg_bragi.hpp>

#include <bragi/helpers-frigg.hpp>
#include <helix/ipc-structs.hpp>

#include <protocols/posix/supercalls.hpp>

extern "C" void __mlibc_signal_restore();

namespace mlibc {

int sys_sigprocmask(int how, const sigset_t *set, sigset_t *retrieve) {
	// This implementation is inherently signal-safe.
	uint64_t former, unused;
	if (set) {
		HEL_CHECK(helSyscall2_2(
		    kHelObserveSuperCall + posix::superSigMask,
		    how,
		    *reinterpret_cast<const HelWord *>(set),
		    &former,
		    &unused
		));
	} else {
		HEL_CHECK(helSyscall2_2(kHelObserveSuperCall + posix::superSigMask, 0, 0, &former, &unused)
		);
	}
	if (retrieve)
		*reinterpret_cast<uint64_t *>(retrieve) = former;
	return 0;
}

int sys_sigaction(
    int number, const struct sigaction *__restrict action, struct sigaction *__restrict saved_action
) {
	SignalGuard sguard;

	// TODO: Respect restorer. __ensure(!(action->sa_flags & SA_RESTORER));

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SIG_ACTION);
	req.set_sig_number(number);
	if (action) {
		req.set_mode(1);
		req.set_flags(action->sa_flags);
		req.set_sig_mask(*reinterpret_cast<const uint64_t *>(&action->sa_mask));
		if (action->sa_flags & SA_SIGINFO) {
			req.set_sig_handler(reinterpret_cast<uintptr_t>(action->sa_sigaction));
		} else {
			req.set_sig_handler(reinterpret_cast<uintptr_t>(action->sa_handler));
		}
		req.set_sig_restorer(reinterpret_cast<uintptr_t>(&__mlibc_signal_restore));
	} else {
		req.set_mode(0);
	}

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() == managarm::posix::Errors::ILLEGAL_REQUEST) {
		// This is only returned for servers, not for normal userspace.
		return ENOSYS;
	} else if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);

	if (saved_action) {
		saved_action->sa_flags = resp.flags();
		*reinterpret_cast<uint64_t *>(&saved_action->sa_mask) = resp.sig_mask();
		if (resp.flags() & SA_SIGINFO) {
			saved_action->sa_sigaction =
			    reinterpret_cast<void (*)(int, siginfo_t *, void *)>(resp.sig_handler());
		} else {
			saved_action->sa_handler = reinterpret_cast<void (*)(int)>(resp.sig_handler());
		}
		// TODO: saved_action->sa_restorer = resp.sig_restorer;
	}
	return 0;
}

int sys_kill(int pid, int number) {
	// This implementation is inherently signal-safe.
	HelWord out;
	HEL_CHECK(helSyscall2_1(kHelObserveSuperCall + posix::superSigKill, pid, number, &out));
	return out;
}

int sys_tgkill(int, int tid, int number) { return sys_kill(tid, number); }

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	HelWord out;

	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall2_1(
	    kHelObserveSuperCall + posix::superSigAltStack,
	    reinterpret_cast<HelWord>(ss),
	    reinterpret_cast<HelWord>(oss),
	    &out
	));

	return out;
}

int sys_sigsuspend(const sigset_t *set) {
	// SignalGuard sguard;
	uint64_t former, seq, unused;

	HEL_CHECK(helSyscall2_2(
	    kHelObserveSuperCall + posix::superSigMask,
	    SIG_SETMASK,
	    *reinterpret_cast<const HelWord *>(set),
	    &former,
	    &seq
	));
	HEL_CHECK(helSyscall1(kHelObserveSuperCall + posix::superSigSuspend, seq));
	HEL_CHECK(helSyscall2_2(
	    kHelObserveSuperCall + posix::superSigMask, SIG_SETMASK, former, &unused, &unused
	));

	return EINTR;
}

int sys_sigpending(sigset_t *set) {
	uint64_t pendingMask;

	HEL_CHECK(helSyscall0_1(kHelObserveSuperCall + posix::superSigGetPending, &pendingMask));
	*reinterpret_cast<uint64_t *>(set) = pendingMask;

	return 0;
}

} // namespace mlibc
