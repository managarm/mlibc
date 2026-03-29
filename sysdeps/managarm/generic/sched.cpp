#include <bits/ensure.h>
#include <unistd.h>

#include <hel-syscalls.h>
#include <hel.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>

#include <posix.frigg_bragi.hpp>

namespace mlibc {

int Sysdeps<GetScheduler>::operator()(pid_t, int *policy) {
	*policy = SCHED_OTHER;
	return 0;
}

int Sysdeps<GetAffinity>::operator()(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	return sysdep<GetThreadaffinity>(pid, cpusetsize, mask);
}

int Sysdeps<GetThreadaffinity>::operator()(pid_t tid, size_t cpusetsize, cpu_set_t *mask) {
	SignalGuard sguard;

	managarm::posix::GetAffinityRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_pid(tid);
	req.set_size(cpusetsize);

	auto [offer, send_head, recv_resp, recv_data] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline(),
	        helix_ng::recvBuffer(mask, cpusetsize)
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else if (resp.error() != managarm::posix::Errors::SUCCESS) {
		mlibc::infoLogger() << "mlibc: got unexpected error from posix in sys_getaffinity!"
		                    << frg::endlog;
		return EIEIO;
	}
	HEL_CHECK(recv_data.error());

	return 0;
}

int Sysdeps<SetAffinity>::operator()(pid_t pid, size_t cpusetsize, const cpu_set_t *mask) {
	return sysdep<SetThreadaffinity>(pid, cpusetsize, mask);
}

int Sysdeps<SetThreadaffinity>::operator()(pid_t tid, size_t cpusetsize, const cpu_set_t *mask) {
	SignalGuard sguard;

	frg::vector<uint8_t, SysdepsAllocator> affinity_mask(getSysdepsAllocator());
	affinity_mask.resize(cpusetsize);
	memcpy(affinity_mask.data(), mask, cpusetsize);
	managarm::posix::SetAffinityRequest<SysdepsAllocator> req(getSysdepsAllocator());

	req.set_pid(tid);
	req.set_mask(affinity_mask);

	auto [offer, send_head, send_tail, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(send_tail.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<SysdepsAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else if (resp.error() != managarm::posix::Errors::SUCCESS) {
		mlibc::infoLogger() << "mlibc: got unexpected error from posix in sys_getaffinity!"
		                    << frg::endlog;
		return EIEIO;
	}

	return 0;
}

int Sysdeps<Getcpu>::operator()(int *cpu) {
	HEL_CHECK(helGetCurrentCpu(cpu));
	return 0;
}

} // namespace mlibc
