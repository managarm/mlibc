#include <errno.h>
#include <string.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <bits/errors.hpp>
#include <bragi/helpers-frigg.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-pipe.hpp>
#include <posix.frigg_bragi.hpp>

namespace mlibc {

int Sysdeps<Mount>::operator()(const char *source, const char *target, const char *fstype, unsigned long, const void *) {
	SignalGuard sguard;

	managarm::posix::MountRequest<SysdepsAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<SysdepsAllocator>(getSysdepsAllocator(), source ? source : ""));
	req.set_target_path(frg::string<SysdepsAllocator>(getSysdepsAllocator(), target ? target : ""));
	req.set_fs_type(frg::string<SysdepsAllocator>(getSysdepsAllocator(), fstype ? fstype : ""));

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

	auto resp =
	    *bragi::parse_head_only<managarm::posix::SvrResponse>(recv_resp, getSysdepsAllocator());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

} // namespace mlibc
