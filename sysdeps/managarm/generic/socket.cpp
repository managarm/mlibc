
#include <bits/ensure.h>
#include <errno.h>
#include <sys/socket.h>

#include <mlibc/allocator.hpp>
#include <mlibc/cxx-support.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <fs.frigg_pb.hpp>
#include <posix.frigg_pb.hpp>

namespace mlibc {

int sys_accept(int fd, int *newfd) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::ACCEPT);
	req.set_fd(fd);

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
	*newfd = resp.fd();
	return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	HelAction actions[5];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_BIND);

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionImbueCredentials;
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionSendFromBuffer;
	actions[3].flags = kHelItemChain;
	actions[3].buffer = const_cast<struct sockaddr *>(addr_ptr);
	actions[3].length = addr_length;
	actions[4].type = kHelActionRecvInline;
	actions[4].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto send_addr = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(send_addr->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return 0;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	HelAction actions[5];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_CONNECT);

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionImbueCredentials;
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionSendFromBuffer;
	actions[3].flags = kHelItemChain;
	actions[3].buffer = const_cast<struct sockaddr *>(addr_ptr);
	actions[3].length = addr_length;
	actions[4].type = kHelActionRecvInline;
	actions[4].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto send_addr = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(send_addr->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return 0;
}

} //namespace mlibc
