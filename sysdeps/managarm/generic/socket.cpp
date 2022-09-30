
#include <bits/ensure.h>
#include <errno.h>
#include <sys/socket.h>

#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <fs.frigg_bragi.hpp>
#include <posix.frigg_bragi.hpp>

namespace mlibc {

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length) {
	SignalGuard sguard;

	managarm::posix::AcceptRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);

	auto [offer, sendReq, recvResp] = exchangeMsgsSync(
		getPosixLane(),
		helix_ng::offer(
			helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			helix_ng::recvInline()
		)
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if(resp.error() == managarm::posix::Errors::WOULD_BLOCK) {
		return EWOULDBLOCK;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*newfd = resp.fd();
	}

	if(addr_ptr && addr_length) {
		if(int e = mlibc::sys_peername(*newfd, addr_ptr, *addr_length, addr_length); e) {
			errno = e;
			return -1;
		}
	}

	return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	HelAction actions[5];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_BIND);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionImbueCredentials;
	actions[2].handle = kHelThisThread;
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
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto send_addr = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(send_addr->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return 0;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_CONNECT);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	auto [offer, send_req, imbue_creds, send_addr, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::sendBuffer(const_cast<struct sockaddr *>(addr_ptr), addr_length),
							helix_ng::recvInline()
					)
			);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(send_addr.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::fs::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return 0;
}

int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_SOCKNAME);
	req.set_fd(fd);
	req.set_size(max_addr_length);

	auto [offer, send_req, recv_resp, recv_addr] = exchangeMsgsSync(
		handle,
		helix_ng::offer(
			helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			helix_ng::recvInline(),
			helix_ng::recvBuffer(addr_ptr, max_addr_length))
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());
	HEL_CHECK(recv_addr.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return ENOTSOCK;
	}
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	HEL_CHECK(recv_addr.error());
	*actual_length = resp.file_size();
	return 0;
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_PEERNAME);
	req.set_fd(fd);
	req.set_size(max_addr_length);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	auto [offer, sendReq, recvResp, recvData] = exchangeMsgsSync(
		handle,
		helix_ng::offer(
			helix_ng::sendBuffer(ser.data(), ser.size()),
			helix_ng::recvInline(),
			helix_ng::recvBuffer(addr_ptr, max_addr_length)
		)
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	if(recvResp.error() == kHelErrDismissed)
		return ENOTSOCK;
	HEL_CHECK(recvResp.error());
	HEL_CHECK(recvData.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return ENOTSOCK;
	}else if(resp.error() == managarm::fs::Errors::NOT_CONNECTED) {
		return ENOTCONN;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*actual_length = resp.file_size();
		return 0;
	}
}

int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	SignalGuard sguard;

	if(layer == SOL_SOCKET && number == SO_PEERCRED) {
		__ensure(*size == sizeof(struct ucred));
		HelAction actions[3];
		globalQueue.trim();

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_OPTION);
		req.set_command(SO_PEERCRED);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		struct ucred creds;
		creds.pid = resp.pid();
		creds.uid = resp.uid();
		creds.gid = resp.gid();
		memcpy(buffer, &creds, sizeof(struct ucred));
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented\e[39m" << frg::endlog;
		*(int *)buffer = 4096;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_TYPE) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is unimplemented, hardcoding SOCK_STREAM\e[39m" << frg::endlog;
		*(int *)buffer = SOCK_STREAM;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_ERROR) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is unimplemented, hardcoding 0\e[39m" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented, hardcoding 0\e[39m" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer << " number: " << number << "\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

int sys_setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) {
	SignalGuard sguard;

	if(layer == SOL_SOCKET && number == SO_PASSCRED) {
		int value;
		__ensure(size == sizeof(int));
		memcpy(&value, buffer, sizeof(int));

		HelAction actions[3];
		globalQueue.trim();

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_SET_OPTION);
		req.set_command(SO_PASSCRED);
		req.set_value(value);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
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
	}else if(layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_REUSEADDR) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_REUSEADDR is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == AF_NETLINK && number == SO_ACCEPTCONN) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with AF_NETLINK and SO_ACCEPTCONN is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected setsockopt() call, layer: " << layer << " number: " << number << "\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

int sys_listen(int fd, int) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_LISTEN);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		handle,
		helix_ng::offer(
			helix_ng::sendBuffer(ser.data(), ser.size()),
			helix_ng::recvInline()
		)
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return 0;
}

} //namespace mlibc
