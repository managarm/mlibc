#include <errno.h>
#include <sys/shm.h>

#include <bits/ensure.h>
#include <bits/errors.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-pipe.hpp>

#include <posix.frigg_bragi.hpp>

namespace mlibc {

int sys_shmget(int *shm_id, key_t key, size_t size, int shmflg) {
	SignalGuard sguard;

	managarm::posix::ShmGetRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_key(key);
	req.set_size(size);
	req.set_flags(shmflg);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::ShmGetResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*shm_id = resp.shmid();
	return 0;
}

int sys_shmat(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	SignalGuard sguard;

	managarm::posix::ShmAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_shmid(shmid);
	req.set_shmaddr(reinterpret_cast<uintptr_t>(shmaddr));
	req.set_flags(shmflg);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::ShmAtResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*seg_start = reinterpret_cast<void *>(resp.address());
	return 0;
}

int sys_shmdt(const void *shmaddr) {
	SignalGuard sguard;

	managarm::posix::ShmDtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_address(reinterpret_cast<uintptr_t>(shmaddr));

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::ShmDtResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_shmctl(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	SignalGuard sguard;

	if (cmd != IPC_STAT && cmd != IPC_RMID) {
		mlibc::infoLogger() << "mlibc: Unsupported sys_shmctl command " << cmd << frg::endlog;
		return EINVAL;
	}

	managarm::posix::ShmCtlRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_shmid(shmid);
	req.set_cmd(cmd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::ShmCtlResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (cmd == IPC_STAT) {
		buf->shm_perm.__ipc_perm_key = resp.perm_key();
		buf->shm_perm.uid = resp.perm_uid();
		buf->shm_perm.gid = resp.perm_gid();
		buf->shm_perm.cuid = resp.perm_cuid();
		buf->shm_perm.cgid = resp.perm_cgid();
		buf->shm_perm.mode = resp.perm_mode();
		buf->shm_perm.__ipc_perm_seq = resp.perm_seq();
		buf->shm_segsz = resp.shm_segsz();
		buf->shm_atime = resp.shm_atime();
		buf->shm_dtime = resp.shm_dtime();
		buf->shm_ctime = resp.shm_ctime();
		buf->shm_cpid = resp.shm_cpid();
		buf->shm_lpid = resp.shm_lpid();
		buf->shm_nattch = resp.shm_nattch();
	}

	// Both IPC_STAT and IPC_RMID return zero on success.
	*idx = 0;
	return 0;
}

} // namespace mlibc
