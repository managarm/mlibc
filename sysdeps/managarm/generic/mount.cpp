
#include <string.h>
#include <errno.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-pipe.hpp>

#include <posix.frigg_pb.hpp>

namespace mlibc {

int sys_mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	SignalGuard sguard;
	HelAction actions[3];

	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::MOUNT);
	req.set_path(frigg::String<MemoryAllocator>(getSysdepsAllocator(), source));
	req.set_target_path(frigg::String<MemoryAllocator>(getSysdepsAllocator(), target));
	req.set_fs_type(frigg::String<MemoryAllocator>(getSysdepsAllocator(), fstype));

	frigg::String<MemoryAllocator> ser(getSysdepsAllocator());
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
	return 0;
}

} //namespace mlibc


