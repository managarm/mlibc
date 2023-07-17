
#include <bits/ensure.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <fs.frigg_bragi.hpp>
#include <posix.frigg_bragi.hpp>

namespace {

int fcntl_helper(int fd, int request, int *result, ...) {
	va_list args;
	va_start(args, result);
	if(!mlibc::sys_fcntl) {
		return ENOSYS;
	}
	int ret = mlibc::sys_fcntl(fd, request, args, result);
	va_end(args);
	return ret;
}

}

namespace mlibc {

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
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

	if(flags & SOCK_NONBLOCK) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFL, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFL, &fcntl_ret, fcntl_ret | O_NONBLOCK);
	}

	if(flags & SOCK_CLOEXEC) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFD, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFD, &fcntl_ret, fcntl_ret | FD_CLOEXEC);
	}

	return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_BIND);

	auto [offer, send_req, send_creds, send_buf, recv_resp] = exchangeMsgsSync(
		handle,
		helix_ng::offer(
			helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			helix_ng::imbueCredentials(),
			helix_ng::sendBuffer(addr_ptr, addr_length),
			helix_ng::recvInline())
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(send_creds.error());
	HEL_CHECK(send_buf.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::fs::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	} else if(resp.error() == managarm::fs::Errors::ADDRESS_IN_USE) {
		return EADDRINUSE;
	} else if(resp.error() == managarm::fs::Errors::ALREADY_EXISTS) {
		return EINVAL;
	} else if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return EINVAL;
	} else if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		return EINVAL;
	} else if(resp.error() == managarm::fs::Errors::ACCESS_DENIED) {
		return EACCES;
	} else if(resp.error() == managarm::fs::Errors::ADDRESS_NOT_AVAILABLE) {
		return EADDRNOTAVAIL;
	}
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
	} else if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		return EINVAL;
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
		if(*size != sizeof(struct ucred))
			return EINVAL;

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_OPTION);
		req.set_command(SO_PEERCRED);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
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

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_SET_OPTION);
		req.set_command(SO_PASSCRED);
		req.set_value(value);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
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
	}else if(layer == SOL_SOCKET && number == SO_RCVBUF) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_RCVBUF is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == IPPROTO_TCP && number == TCP_NODELAY) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_NODELAY is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_ACCEPTCONN) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_ACCEPTCONN is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == IPPROTO_TCP && number == TCP_KEEPIDLE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_KEEPIDLE is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_NETLINK && number == NETLINK_EXT_ACK) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_NETLINK and NETLINK_EXT_ACK is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_NETLINK && number == NETLINK_GET_STRICT_CHK) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_NETLINK and NETLINK_EXT_ACK is unimplemented\e[39m" << frg::endlog;
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
