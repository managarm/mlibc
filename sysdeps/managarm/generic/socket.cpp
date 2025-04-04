#include <array>
#include <asm/socket.h>
#include <bits/ensure.h>
#include <errno.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <bits/errors.hpp>
#include <fs.frigg_bragi.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <posix.frigg_bragi.hpp>

namespace {

int fcntl_helper(int fd, int request, int *result, ...) {
	va_list args;
	va_start(args, result);
	if (!mlibc::sys_fcntl) {
		return ENOSYS;
	}
	int ret = mlibc::sys_fcntl(fd, request, args, result);
	va_end(args);
	return ret;
}

} // namespace

namespace mlibc {

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	SignalGuard sguard;

	managarm::posix::AcceptRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);

	auto [offer, sendReq, recvResp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS) {
		return resp.error() | toErrno;
	} else {
		*newfd = resp.fd();
	}

	if (addr_ptr && addr_length) {
		if (int e = mlibc::sys_peername(*newfd, addr_ptr, *addr_length, addr_length); e) {
			errno = e;
			return -1;
		}
	}

	if (flags & SOCK_NONBLOCK) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFL, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFL, &fcntl_ret, fcntl_ret | O_NONBLOCK);
	}

	if (flags & SOCK_CLOEXEC) {
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
	        helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(send_creds.error());
	HEL_CHECK(send_buf.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	return resp.error() | toErrno;
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

	auto [offer, send_req, imbue_creds, send_addr, recv_resp] = exchangeMsgsSync(
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
	return resp.error() | toErrno;
}

int sys_sockname(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
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
	        helix_ng::recvBuffer(addr_ptr, max_addr_length)
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() == managarm::fs::Errors::SUCCESS) {
		HEL_CHECK(recv_addr.error());
		*actual_length = resp.file_size();
		return 0;
	}
	return resp.error() | toErrno;
}

int sys_peername(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
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
	if (recvResp.error() == kHelErrDismissed)
		return ENOTSOCK;
	HEL_CHECK(recvResp.error());
	HEL_CHECK(recvData.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());

	if (resp.error() == managarm::fs::Errors::SUCCESS) {
		*actual_length = resp.file_size();
		return 0;
	}

	return resp.error() | toErrno;
}

namespace {

std::array<std::pair<int, int>, 6> getsockopt_passthrough = {{
    {SOL_SOCKET, SO_PROTOCOL},
    {SOL_SOCKET, SO_PEERCRED},
    {SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS},
    {SOL_SOCKET, SO_TYPE},
    {SOL_SOCKET, SO_ACCEPTCONN},
    {SOL_SOCKET, SO_PEERPIDFD},
}};

}

int
sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	SignalGuard sguard;

	if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		// This is really only relevant on Linux
		*(int *)buffer = 4096;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_RCVBUF) {
		// This is really only relevant on Linux
		*(int *)buffer = 4096;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_ERROR) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is "
		                       "unimplemented, hardcoding 0\e[39m"
		                    << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_KEEPALIVE is "
		                       "unimplemented, hardcoding 0\e[39m"
		                    << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_LINGER) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_LINGER is "
		                       "unimplemented, hardcoding 0\e[39m"
		                    << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_PEERSEC) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_PEERSEC is "
		                       "unimplemented, hardcoding 0\e[39m"
		                    << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_PEERGROUPS) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_PEERGROUPS "
		                       "is unimplemented, hardcoding 0\e[39m"
		                    << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_MAXSEG) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with IPPROTO_TCP and TCP_MAXSEG is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_CONGESTION) {
		mlibc::infoLogger(
		) << "\e[31mmlibc: getsockopt() call with IPPROTO_TCP and TCP_CONGESTION is "
		     "unimplemented\e[39m"
		  << frg::endlog;
		return 0;
	} else if (std::find(
	               getsockopt_passthrough.begin(),
	               getsockopt_passthrough.end(),
	               std::pair<int, int>{layer, number}
	           )
	           != getsockopt_passthrough.end()) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::GetSockOpt<MemoryAllocator> req(getSysdepsAllocator());
		req.set_layer(layer);
		req.set_number(number);
		req.set_optlen(size ? *size : 0);

		auto [offer, send_req, send_creds, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::want_lane,
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
		        helix_ng::imbueCredentials(),
		        helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		*size = resp.size();

		auto [recv_buffer] =
		    exchangeMsgsSync(offer.descriptor().getHandle(), helix_ng::recvBuffer(buffer, *size));
		HEL_CHECK(recv_buffer.error());

		return resp.error() | toErrno;
	} else {
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer
		                     << " number: " << number << "\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

namespace {

std::array<std::pair<int, int>, 6> setsockopt_readonly = {{
    {SOL_SOCKET, SO_ACCEPTCONN},
    {SOL_SOCKET, SO_DOMAIN},
    {SOL_SOCKET, SO_ERROR},
    {SOL_SOCKET, SO_PROTOCOL},
    {SOL_SOCKET, SO_TYPE},
    {SOL_IP, SO_PEERSEC},
}};

std::array<std::pair<int, int>, 12> setsockopt_passthrough = {{
    {SOL_PACKET, PACKET_AUXDATA},
    {SOL_SOCKET, SO_LOCK_FILTER},
    {SOL_SOCKET, SO_BINDTODEVICE},
    {SOL_SOCKET, SO_TIMESTAMP},
    {SOL_SOCKET, SO_PASSCRED},
    {SOL_SOCKET, SO_RCVTIMEO},
    {SOL_SOCKET, SO_SNDTIMEO},
    {SOL_IP, IP_PKTINFO},
    {SOL_IP, IP_RECVTTL},
    {SOL_IP, IP_RETOPTS},
    {SOL_NETLINK, NETLINK_ADD_MEMBERSHIP},
    {SOL_NETLINK, NETLINK_PKTINFO},
}};

std::array<std::pair<int, int>, 2> setsockopt_passthrough_noopt = {{
    {SOL_SOCKET, SO_DETACH_FILTER},
}};

} // namespace

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	SignalGuard sguard;

	if (std::find(
	        setsockopt_passthrough.begin(),
	        setsockopt_passthrough.end(),
	        std::pair<int, int>{layer, number}
	    )
	    != setsockopt_passthrough.end()) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::SetSockOpt<MemoryAllocator> req(getSysdepsAllocator());
		req.set_layer(layer);
		req.set_number(number);
		req.set_optlen(size);

		auto [offer, send_req, send_buf, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
		        helix_ng::sendBuffer(buffer, size),
		        helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_buf.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		return resp.error() | toErrno;
	} else if (std::find(
	               setsockopt_passthrough_noopt.begin(),
	               setsockopt_passthrough_noopt.end(),
	               std::pair<int, int>{layer, number}
	           )
	           != setsockopt_passthrough_noopt.end()) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::SetSockOpt<MemoryAllocator> req(getSysdepsAllocator());
		req.set_layer(layer);
		req.set_number(number);
		req.set_optlen(0);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		return resp.error() | toErrno;
	} else if (std::find(
	               setsockopt_readonly.begin(),
	               setsockopt_readonly.end(),
	               std::pair<int, int>{layer, number}
	           )
	           != setsockopt_readonly.end()) {
		// this is purely read-only
		return ENOPROTOOPT;
	} else if (layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		if (size != sizeof(sock_fprog))
			return EINVAL;

		auto fprog = reinterpret_cast<const sock_fprog *>(buffer);

		managarm::fs::SetSockOpt<MemoryAllocator> req(getSysdepsAllocator());
		req.set_layer(layer);
		req.set_number(number);
		req.set_optlen(fprog->len * sizeof(*fprog->filter));

		auto [offer, send_req, send_buf, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
		        helix_ng::sendBuffer(fprog->filter, req.optlen()),
		        helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_buf.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		return resp.error() | toErrno;
	} else if (layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_RCVBUFFORCE) is not implemented"
		                       " correctly\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		// This is really only relevant on Linux
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUFFORCE) {
		// This is really only relevant on Linux
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_KEEPALIVE is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_REUSEADDR) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_REUSEADDR is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_REUSEPORT) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_REUSEPORT is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_RCVBUF) {
		mlibc::infoLogger(
		) << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_RCVBUF is unimplemented\e[39m"
		  << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_NODELAY) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_NODELAY is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_MAXSEG) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_NODELAY is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_KEEPIDLE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_KEEPIDLE "
		                       "is unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_NETLINK && number == NETLINK_BROADCAST_ERROR) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_NETLINK and "
		                       "NETLINK_BROADCAST_ERROR is unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_NETLINK && number == NETLINK_EXT_ACK) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_NETLINK and "
		                       "NETLINK_EXT_ACK is unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_NETLINK && number == NETLINK_GET_STRICT_CHK) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_NETLINK and "
		                       "NETLINK_EXT_ACK is unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_KEEPINTVL) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_KEEPINTVL "
		                       "is unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == IPPROTO_TCP && number == TCP_KEEPCNT) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with IPPROTO_TCP and TCP_KEEPCNT is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_OOBINLINE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_OOBINLINE is "
		                       "unimplemented\e[39m"
		                    << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_PRIORITY) {
		mlibc::infoLogger(
		) << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_PRIORITY is unimplemented\e[39m"
		  << frg::endlog;
		return 0;
	} else if (layer == SOL_IP && number == IP_RECVERR) {
		mlibc::infoLogger(
		) << "\e[31mmlibc: setsockopt() call with SOL_IP and IP_RECVERR is unimplemented\e[39m"
		  << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_PASSSEC) {
		mlibc::infoLogger(
		) << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_PASSSEC is unimplemented\e[39m"
		  << frg::endlog;
		return ENOSYS;
	} else {
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected setsockopt() call, layer: " << layer
		                     << " number: " << number << "\e[39m" << frg::endlog;
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
	    helix_ng::offer(helix_ng::sendBuffer(ser.data(), ser.size()), helix_ng::recvInline())
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	return resp.error() | toErrno;
}

} // namespace mlibc
