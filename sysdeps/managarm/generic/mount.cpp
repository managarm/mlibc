#include <errno.h>
#include <string.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <posix.frigg_bragi.hpp>
#include <bragi/helpers-frigg.hpp>

namespace mlibc {

int sys_mount(const char *source, const char *target,
		const char *fstype, unsigned long, const void *) {
	SignalGuard sguard;

	managarm::posix::MountRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), source));
	req.set_target_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), target));
	req.set_fs_type(frg::string<MemoryAllocator>(getSysdepsAllocator(), fstype));

	auto [offer, send_head, send_tail, recv_resp] =
		exchangeMsgsSync(
			getPosixLane(),
			helix_ng::offer(
				helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(send_tail.error());
	HEL_CHECK(recv_resp.error());

	auto resp = *bragi::parse_head_only<managarm::posix::SvrResponse>(recv_resp, getSysdepsAllocator());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND)
		return ENOENT;
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}

} //namespace mlibc
