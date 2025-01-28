#pragma once

#include <ifaddrs.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

#include <frg/allocation.hpp>
#include <frg/span.hpp>
#include <unistd.h>

#include "mlibc/allocator.hpp"
#include "mlibc/posix-sysdeps.hpp"

namespace {

/**
 * Helper class for issuing rtnetlink requests and receiving replies.
 */
struct NetlinkHelper {
	~NetlinkHelper() {
		if(fd_)
			close(*fd_);
	}

	bool send_request(int type) {
		if(!fd_ || *fd_ == -1) {
			int fd;
			if(int e = mlibc::sys_socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE, &fd); e)
				return false;
			fd_ = fd;
		}

		struct NetlinkMessage {
			nlmsghdr hdr;
			rtgenmsg msg;
		} request;

		memset(&request, 0, sizeof(request));
		request.hdr.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
		request.hdr.nlmsg_type = type;
		request.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(request) - sizeof(nlmsghdr));
		request.msg.rtgen_family = AF_UNSPEC;

		return send(fd_.value(), reinterpret_cast<void *>(&request), sizeof(request), 0) == sizeof(request);
	}

	bool recv(void cb(void *, nlmsghdr *), void *ctx) {
		ssize_t read;

		while((read = ::recv(fd_.value(), reinterpret_cast<msghdr *>(data_.data()), data_.size(), 0)) > 0) {
			auto hdr = reinterpret_cast<nlmsghdr *>(data_.data());

			for(; NLMSG_OK(hdr, static_cast<size_t>(read)); hdr = NLMSG_NEXT(hdr, read)) {
				if(hdr->nlmsg_type == NLMSG_DONE)
					return true;
				if(hdr->nlmsg_type == NLMSG_ERROR) {
					nlmsgerr *err = reinterpret_cast<nlmsgerr *>(NLMSG_DATA(hdr));
					mlibc::infoLogger() << "mlibc: NetlinkHelper received NLMSG_ERROR " << -err->error << ": "
						<< strerror(-err->error) << frg::endlog;
					return false;
				}

				cb(ctx, hdr);
			}
		}

		return false;
	}
private:
	frg::optional<int> fd_;
	frg::array<uint8_t, 8192> data_;
};

/**
 * Helper class for constructing `struct ifaddrs` with data from rtnetlink.
 *
 * This can be used for implementing `sys_getifaddrs` if the OS supports rtnetlink.
 */
struct IfaddrHelper {
	ifaddrs ifa = {};

	int if_index_;

	sockaddr_storage addr_;
	sockaddr_storage netmask_;
	sockaddr_storage broadcast_;
	sockaddr_storage dest_addr_;

	char name_[IFNAMSIZ + 1];

public:
	explicit IfaddrHelper(ifaddrs **list) {
		// append ourselves to the end of the chain
		ifaddrs *last = *list;

		if(last) {
			while(last->ifa_next)
				last = last->ifa_next;

			last->ifa_next = &this->ifa;
		} else {
			*list = &this->ifa;
		}
	}

	void set_broadcast_addr(int family, const void *data, size_t byteCount) {
		ifa.ifa_broadaddr = copy_addr(family, data, byteCount, &broadcast_);
	}

	/**
	 * Handler for IFA_ADDRESS.
	 *
	 * Sets the destination address, and provides the fallback for the local address if
	 * it is not separately specified, via IFA_LOCAL and its handler.
	 */
	void set_addr(int family, const void *data, size_t bytes) {
		if(ifa.ifa_addr == nullptr)
			ifa.ifa_addr = copy_addr(family, data, bytes, &addr_);

		ifa.ifa_dstaddr = copy_addr(family, data, bytes, &dest_addr_);
	}

	/**
	 * Handler for IFA_LOCAL.
	 *
	 * Also sets the destination address, if appropriate; this will be overridden if a
	 * destination address is specified, via IFA_ADDRESS and its handler.
	 */
	void set_local_address(int family, const void *data, size_t byteCount) {
		// rtnetlink distinguishes the addresses IFA_ADDRESS and IFA_LOCAL
		// - IFA_LOCAL is the local interface address.
		// - IFA_ADDRESS is the prefix address:
		//   - for a point-to-point connection, this is the destination address,
		//   - while for a normally configured broadcast interface, it is the same as IFA_LOCAL.

		// if we already have an ifa_addr, move it over to ifa_dstaddr
		if(ifa.ifa_addr != nullptr)
			ifa.ifa_dstaddr = reinterpret_cast<sockaddr *>(memcpy(&dest_addr_, &addr_, sizeof(addr_)));

		ifa.ifa_addr = copy_addr(family, data, byteCount, &addr_);
	}

	void set_netmask(int family, size_t prefix_length) {
		netmask_.ss_family = family;

		auto dst = sockaddr_bytes(family, &netmask_);
		// set as many complete bytes as needed to 255
		memset(dst.data(), 0xFF, prefix_length / 8);

		// if needed, set the last partial byte to its correct mask
		if(prefix_length % 8)
			dst[prefix_length/8] = (0xFF << (8 - (prefix_length % 8))) & 0xFF;

		ifa.ifa_netmask = reinterpret_cast<sockaddr *>(&netmask_);
	}

	void set_packet_attrs(int ifindex, unsigned short hatype, unsigned char halen) {
		sockaddr_ll *sll = reinterpret_cast<sockaddr_ll *>(&addr_);
		sll->sll_ifindex = ifindex;
		sll->sll_hatype = hatype;
		sll->sll_halen = halen;
	}

private:
	/**
	 * Copy an address to its storage, and return a pointer to that.
	 */
	sockaddr *copy_addr(sa_family_t family, const void *data, size_t byteCount, sockaddr_storage *ss) {
		ss->ss_family = family;

		auto sb = sockaddr_bytes(family, ss);
		memcpy(sb.data(), data, frg::min(byteCount, sb.size()));

		if(family == AF_INET6 && (IN6_IS_ADDR_LINKLOCAL(data) || IN6_IS_ADDR_MC_LINKLOCAL(data))) {
			reinterpret_cast<sockaddr_in6 *>(ss)->sin6_scope_id = if_index_;
		}

		return reinterpret_cast<sockaddr *>(ss);
	}

	/**
	 * Retrieve an appropriately sized span for the sockaddr's addr
	 */
	frg::span<uint8_t> sockaddr_bytes(int family, sockaddr_storage *ss) {
		if(family == AF_INET) {
			sockaddr_in *ss4 = reinterpret_cast<sockaddr_in *>(ss);
			return {reinterpret_cast<uint8_t *>(&ss4->sin_addr), sizeof(ss4->sin_addr)};
		} else if(family == AF_INET6) {
			sockaddr_in6 *ss6 = reinterpret_cast<sockaddr_in6 *>(ss);
			return {reinterpret_cast<uint8_t *>(&ss6->sin6_addr), sizeof(ss6->sin6_addr)};
		} else if(family == AF_PACKET) {
			sockaddr_ll *sll = reinterpret_cast<sockaddr_ll *>(ss);
			return {reinterpret_cast<uint8_t *>(&sll->sll_addr), sizeof(sll->sll_addr)};
		} else {
			mlibc::panicLogger() << "unimplemented family " << family << frg::endlog;
			__builtin_unreachable();
		}
	}
};

// We rely on the appended ifaddrs struct to actually be a IfaddrHelper
static_assert(offsetof(IfaddrHelper, ifa) == 0);

/**
 * Callback function to be used with NetlinkHelper for implementing `sys_getifaddrs` by using rtnetlink.
 */
void getifaddrs_callback(void *context, nlmsghdr *hdr) {
	ifaddrs **out = reinterpret_cast<ifaddrs **>(context);

	if(hdr->nlmsg_type == RTM_NEWLINK) {
		ifinfomsg *ifi = reinterpret_cast<ifinfomsg *>(NLMSG_DATA(hdr));

		auto new_addr = frg::construct<IfaddrHelper>(getAllocator(), out);
		new_addr->if_index_ = ifi->ifi_index;
		new_addr->ifa.ifa_flags = ifi->ifi_flags;

		rtattr *rta = IFLA_RTA(ifi);
		size_t rta_len = IFLA_PAYLOAD(hdr);

		while(RTA_OK(rta, rta_len)) {
			if(rta->rta_type == IFLA_ADDRESS) {
				if(RTA_PAYLOAD(rta) < sizeof(new_addr->addr_)) {
					new_addr->set_addr(AF_PACKET, RTA_DATA(rta), RTA_PAYLOAD(rta));
					new_addr->set_packet_attrs(ifi->ifi_index, ifi->ifi_type, RTA_PAYLOAD(rta));
				}
			} else if(rta->rta_type == IFLA_BROADCAST) {
				if(RTA_PAYLOAD(rta) < sizeof(new_addr->broadcast_)) {
					new_addr->set_broadcast_addr(AF_PACKET, RTA_DATA(rta), RTA_PAYLOAD(rta));
					new_addr->set_packet_attrs(ifi->ifi_index, ifi->ifi_type, RTA_PAYLOAD(rta));
				}
			} else if(rta->rta_type == IFLA_IFNAME) {
				if(RTA_PAYLOAD(rta) < sizeof(new_addr->name_)) {
					memcpy(new_addr->name_, RTA_DATA(rta), RTA_PAYLOAD(rta));
					new_addr->ifa.ifa_name = new_addr->name_;
				}
			}

			rta = RTA_NEXT(rta, rta_len);
		}
	} else if(hdr->nlmsg_type == RTM_NEWADDR) {
		ifaddrmsg *msg = reinterpret_cast<ifaddrmsg*>(NLMSG_DATA(hdr));

		const IfaddrHelper *current = reinterpret_cast<const IfaddrHelper *>(*out);
		while (current != nullptr && current->if_index_ != static_cast<int>(msg->ifa_index)) {
			current = reinterpret_cast<const IfaddrHelper*>(current->ifa.ifa_next);
		}

		IfaddrHelper *new_addr = frg::construct<IfaddrHelper>(getAllocator(), out);
		new_addr->if_index_ = static_cast<int>(msg->ifa_index);

		if(current != nullptr) {
			strcpy(new_addr->name_, current->name_);
			new_addr->ifa.ifa_name = new_addr->name_;
			new_addr->ifa.ifa_flags = current->ifa.ifa_flags;
		}

		rtattr *rta = IFA_RTA(msg);
		size_t rta_len = IFA_PAYLOAD(hdr);

		while(RTA_OK(rta, rta_len)) {
			if(rta->rta_type == IFA_ADDRESS) {
				if(msg->ifa_family == AF_INET || msg->ifa_family == AF_INET6) {
					new_addr->set_addr(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
					new_addr->set_netmask(msg->ifa_family, msg->ifa_prefixlen);
				}
			} else if(rta->rta_type == IFA_BROADCAST) {
				if(msg->ifa_family == AF_INET) {
					new_addr->set_broadcast_addr(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
					if(current == nullptr) {
						new_addr->ifa.ifa_flags |= IFF_BROADCAST;
					}
				}
			} else if(rta->rta_type == IFA_LOCAL) {
				if(msg->ifa_family == AF_INET || msg->ifa_family == AF_INET6) {
					new_addr->set_local_address(msg->ifa_family, RTA_DATA(rta), RTA_PAYLOAD(rta));
				}
			} else if(rta->rta_type == IFA_LABEL) {
				if(RTA_PAYLOAD(rta) < sizeof(new_addr->name_)) {
					memcpy(new_addr->name_, RTA_DATA(rta), RTA_PAYLOAD(rta));
					new_addr->ifa.ifa_name = new_addr->name_;
				}
			}

			rta = RTA_NEXT(rta, rta_len);
		}
	}
}

} // namespace
