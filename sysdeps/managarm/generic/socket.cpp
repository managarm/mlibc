
#include <bits/ensure.h>
#include <errno.h>
#include <sys/socket.h>

#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
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

int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	HelAction actions[4];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_SOCKNAME);
	req.set_fd(fd);
	req.set_size(max_addr_length);

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionRecvToBuffer;
	actions[3].flags = 0;
	actions[3].buffer = addr_ptr;
	actions[3].length = max_addr_length;
	HEL_CHECK(helSubmitAsync(handle, actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_addr = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_addr->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	*actual_length = resp.file_size();
	return 0;
}

int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	if(layer == SOL_SOCKET && number == SO_PEERCRED) {
		__ensure(*size == sizeof(struct ucred));
		HelAction actions[3];
		globalQueue.trim();

		auto handle = cacheFileTable()[fd];
		__ensure(handle);

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_OPTION);
		req.set_command(SO_PEERCRED);

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
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		struct ucred creds;
		creds.pid = resp.pid();
		creds.uid = resp.uid();
		creds.gid = resp.gid();
		memcpy(buffer, &creds, sizeof(struct ucred));
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected getsockopt() call\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

int sys_setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) {
	if(layer == SOL_SOCKET && number == SO_PASSCRED) {
		int value;
		__ensure(size == sizeof(int));
		memcpy(&value, buffer, sizeof(int));

		HelAction actions[3];
		globalQueue.trim();

		auto handle = cacheFileTable()[fd];
		__ensure(handle);

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_SET_OPTION);
		req.set_command(SO_PASSCRED);
		req.set_value(value);

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
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_ATTACH_FILTER) is not implemented"
				" correctly\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_RCVBUFFORCE) is not implemented"
				" correctly\e[39m" << frg::endlog;
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected setsockopt() call\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

} //namespace mlibc
