
#include <string.h>
#include <errno.h>

#include <sys/auxv.h>

// for dup2()
#include <unistd.h>
#include <dirent.h>
// for open()
#include <fcntl.h>
// for tcgetattr()
#include <termios.h>
// for mmap()
#include <sys/mman.h>
// for stat()
#include <sys/stat.h>
#include <sys/inotify.h>
// for ioctl()
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <sys/sysmacros.h>
#include <linux/input.h>
#include <libdrm/drm.h>
#include <libdrm/drm_fourcc.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/cxx-support.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/sysdeps.hpp>

#include <frigg/vector.hpp>
#include <frigg/hashmap.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>
#include <fs.frigg_pb.hpp>

#include <mlibc/sysdeps.hpp>

HelHandle __mlibc_getPassthrough(int fd) {
	auto handle = cacheFileTable()[fd];
	__ensure(handle);
	return handle;
}

namespace mlibc {

int sys_chroot(const char *path) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::CHROOT);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	return 0;
}

int sys_mkdir(const char *path) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::MKDIR);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		errno = EEXIST;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_symlink(const char *target_path, const char *link_path) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::SYMLINK);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), link_path));
	req.set_target_path(frigg::String<MemoryAllocator>(getAllocator(), target_path));

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
	return 0;
}

int sys_rename(const char *path, const char *new_path) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::RENAME);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));
	req.set_target_path(frigg::String<MemoryAllocator>(getAllocator(), new_path));

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
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

} //namespace mlibc

HelHandle __raw_map(int fd) {
	HelAction actions[4];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);
	
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::MMAP);
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
	actions[2].flags = kHelItemChain;
	actions[3].type = kHelActionPullDescriptor;
	actions[3].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto pull_memory = parseHandle(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(pull_memory->error);
	
	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return pull_memory->handle;
}

namespace mlibc {

int sys_fcntl(int fd, int request, va_list args) {
	if(request == F_DUPFD) {
		int newfd;
		if(sys_dup(fd, 0, &newfd))
			return -1;
		return newfd;
	}else if(request == F_DUPFD_CLOEXEC) {
		int newfd;
		if(sys_dup(fd, __MLIBC_O_CLOEXEC, &newfd))
			return -1;
		return newfd;
	}else if(request == F_GETFD) {
		HelAction actions[3];
		globalQueue.trim();

		managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_request_type(managarm::posix::CntReqType::FD_GET_FLAGS);
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
		return resp.flags();
	}else if(request == F_SETFD) {
		frigg::infoLogger() << "\e[31mmlibc: fcntl(F_SETFD) is not implemented correctly"
				<< "\e[39m" << frigg::endLog;
		return 0;
	}else{
		frigg::infoLogger() << "\e[31mmlibc: Unexpected fcntl() request: "
				<< request << "\e[39m" << frigg::endLog;
		errno = EINVAL;
		return -1;
	}
}

int sys_open_dir(const char *path, int *handle) {
	if((*handle = open(path, 0)) == -1)
		return -1;
	return 0;
}

int sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read) {
	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	HelAction actions[3];
	globalQueue.trim();
	
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_READ_ENTRIES);

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
	if(resp.error() == managarm::fs::Errors::END_OF_FILE) {
		*bytes_read = 0;
		return 0;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		__ensure(max_size > sizeof(struct dirent));
		auto ent = new (buffer) struct dirent;
		memset(ent, 0, sizeof(struct dirent));
		memcpy(ent->d_name, resp.path().data(), resp.path().size());
		ent->d_reclen = sizeof(struct dirent);
		*bytes_read = sizeof(struct dirent);
		return 0;
	}
}

int sys_ttyname(int fd, char *buf, size_t size) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::TTY_NAME);
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
	if(resp.error() ==  managarm::posix::Errors::BAD_FD) {
		errno = ENOTTY;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		__ensure(size >= resp.path().size() + 1);
		memcpy(buf, resp.path().data(), size);
		buf[resp.path().size()] = '\0';
		return 0;
	}
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	__ensure(!hint);

	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_MAP);
	req.set_size(size);
	req.set_mode(prot);
	req.set_flags(flags);
	req.set_fd(fd);
	req.set_rel_offset(offset);

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
	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_REMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_new_size(new_size);

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
	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_UNMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);

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
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	frigg::infoLogger() << "mlibc: Broken tcgetattr() called!" << frigg::endLog;
	attr->c_iflag = 0;
	attr->c_oflag = 0;
	attr->c_cflag = 0;
	attr->c_lflag = ECHO;
	for(size_t i = 0; i < NCCS; i++)
		attr->c_cc[i] = 0;
	attr->c_cc[VMIN] = 1;
	attr->c_cc[VTIME] = 0;
	return 0;
}

int sys_tcsetattr(int, int, const struct termios *attr) {
	frigg::infoLogger() << "mlibc: Broken tcsetattr("
			<< attr->c_iflag << ", " << attr->c_oflag
			<< ", " << attr->c_cflag << ", " << attr->c_lflag
			<< ") called!" << frigg::endLog;
	return 0;
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
	constexpr int type_mask = int(0xFFFF);
	constexpr int flags_mask = ~int(0xFFFF);
	__ensure(!((type_and_flags & flags_mask) & ~(SOCK_CLOEXEC | SOCK_NONBLOCK)));

	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::SOCKET);
	req.set_domain(domain);
	req.set_socktype(type_and_flags & type_mask);
	req.set_protocol(proto);
	req.set_flags(type_and_flags & flags_mask);

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
	*fd = resp.fd();
	return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	constexpr int type_mask = int(0xFFFF);
	constexpr int flags_mask = ~int(0xFFFF);
	__ensure(!((type_and_flags & flags_mask) & ~(SOCK_CLOEXEC | SOCK_NONBLOCK)));

	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::SOCKPAIR);
	req.set_domain(domain);
	req.set_socktype(type_and_flags & type_mask);
	req.set_protocol(proto);
	req.set_flags(type_and_flags & flags_mask);

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
	__ensure(resp.fds_size() == 2);
	fds[0] = resp.fds(0);
	fds[1] = resp.fds(1);
	return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length) {
	__ensure(hdr->msg_iovlen);

	HelAction actions[5];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::SENDMSG);
	req.set_fd(sockfd);

	for(auto cmsg = CMSG_FIRSTHDR(hdr); cmsg; cmsg = CMSG_NXTHDR(hdr, cmsg)) {
		__ensure(cmsg->cmsg_level == SOL_SOCKET);
		__ensure(cmsg->cmsg_type == SCM_RIGHTS);
		__ensure(cmsg->cmsg_len >= sizeof(struct cmsghdr));
		
		size_t size = cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr));
		__ensure(!(size % sizeof(int)));
		for(size_t off = 0; off < size; off += sizeof(int)) {
			int fd;
			memcpy(&fd, CMSG_DATA(cmsg) + off, sizeof(int));
			req.add_fds(fd);
		}
	}

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionSendFromBuffer;
	actions[2].flags = kHelItemChain;
	actions[2].buffer = hdr->msg_iov[0].iov_base;
	actions[2].length = hdr->msg_iov[0].iov_len;
	actions[3].type = kHelActionSendFromBuffer;
	actions[3].flags = kHelItemChain;
	actions[3].buffer = hdr->msg_name;
	actions[3].length = hdr->msg_namelen;
	actions[4].type = kHelActionRecvInline;
	actions[4].flags = 0;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto send_data = parseSimple(element);
	auto send_addr = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(send_data->error);
	HEL_CHECK(send_addr->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*length = resp.size();
	return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
	__ensure(hdr->msg_iovlen);

	HelAction actions[6];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::RECVMSG);
	req.set_fd(sockfd);
	req.set_size(hdr->msg_iov[0].iov_len);
	req.set_addr_size(hdr->msg_namelen);
	req.set_ctrl_size(hdr->msg_controllen);

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
	actions[3].flags = kHelItemChain;
	actions[3].buffer = hdr->msg_name;
	actions[3].length = hdr->msg_namelen;
	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = kHelItemChain;
	actions[4].buffer = hdr->msg_iov[0].iov_base;
	actions[4].length = hdr->msg_iov[0].iov_len;
	actions[5].type = kHelActionRecvToBuffer;
	actions[5].flags = 0;
	actions[5].buffer = hdr->msg_control;
	actions[5].length = hdr->msg_controllen;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 6,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_addr = parseLength(element);
	auto recv_data = parseLength(element);
	auto recv_ctrl = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_addr->error);
	HEL_CHECK(recv_data->error);
	HEL_CHECK(recv_ctrl->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);

	hdr->msg_namelen = recv_addr->length;
	hdr->msg_controllen = recv_ctrl->length;
	*length = recv_data->length;
	return 0;
}

int sys_epoll_create(int flags, int *fd) {
	__ensure(!(flags & ~(EPOLL_CLOEXEC)));

	HelAction actions[3];
	globalQueue.trim();
	
	uint32_t proto_flags = 0;
	if(flags & EPOLL_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_CREATE);
	req.set_flags(proto_flags);

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
	*fd = resp.fd();
	return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	if(mode == EPOLL_CTL_ADD) {
		__ensure(ev);
		req.set_request_type(managarm::posix::CntReqType::EPOLL_ADD);
		req.set_flags(ev->events);
		req.set_cookie(ev->data.u64);
	}else if(mode == EPOLL_CTL_MOD) {
		__ensure(ev);
		req.set_request_type(managarm::posix::CntReqType::EPOLL_MODIFY);
		req.set_flags(ev->events);
		req.set_cookie(ev->data.u64);
	}else if(mode == EPOLL_CTL_DEL) {
		__ensure(!ev);
		req.set_request_type(managarm::posix::CntReqType::EPOLL_DELETE);
	}else{
		frigg::panicLogger() << "\e[31mmlibc: Illegal epoll_ctl() mode\e[39m" << frigg::endLog;
	}
	req.set_fd(epfd);
	req.set_newfd(fd);

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
	return 0;
}

int sys_epoll_wait(int epfd, struct epoll_event *evnts, int n, int timeout) {
	__ensure(timeout >= 0 || timeout == -1); // TODO: Report errors correctly.

	HelAction actions[4];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_WAIT);
	req.set_fd(epfd);
	req.set_size(n);
	req.set_timeout(timeout > 0 ? int64_t{timeout} * 1000000 : timeout);

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
	actions[3].buffer = evnts;
	actions[3].length = n * sizeof(struct epoll_event);
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_data->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	__ensure(!(recv_data->length % sizeof(struct epoll_event)));
	return recv_data->length / sizeof(struct epoll_event);
}

int sys_timerfd_create(int flags, int *fd) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::TIMERFD_CREATE);
	req.set_flags(flags);

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
	*fd = resp.fd();
	return 0;
}

int sys_timerfd_settime(int fd, int flags,
		const struct itimerspec *value) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::TIMERFD_SETTIME);
	req.set_fd(fd);
	req.set_time_secs(value->it_value.tv_sec);
	req.set_time_nanos(value->it_value.tv_nsec);
	req.set_interval_secs(value->it_interval.tv_sec);
	req.set_interval_nanos(value->it_interval.tv_nsec);

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
	return 0;
}

int sys_signalfd_create(int flags, int *fd) {
	__ensure(!(flags & ~(SFD_CLOEXEC | SFD_NONBLOCK)));

	if(flags & SFD_NONBLOCK)
		frigg::infoLogger() << "\e[31mmlibc: signalfd(SFD_NONBLOCK)"
				" is not implemented correctly\e[39m" << frigg::endLog;
	
	uint32_t proto_flags = 0;
	if(flags & SFD_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::SIGNALFD_CREATE);
	req.set_flags(proto_flags);

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
	*fd = resp.fd();
	return 0;
}

int sys_inotify_create(int flags, int *fd) {
	__ensure(!(flags & ~(IN_CLOEXEC)));

	HelAction actions[3];
	globalQueue.trim();
	
	uint32_t proto_flags = 0;
	if(flags & IN_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::INOTIFY_CREATE);
	req.set_flags(proto_flags);

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
	*fd = resp.fd();
	return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg) {
//	frigg::infoLogger() << "mlibc: ioctl with"
//			<< " type: 0x" << frigg::logHex(_IOC_TYPE(request))
//			<< ", number: 0x" << frigg::logHex(_IOC_NR(request))
//			<< " (raw request: " << frigg::logHex(request) << ")"
//			<< " on fd " << fd << frigg::endLog;

	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	switch(request) {
	case DRM_IOCTL_VERSION: {
		auto param = reinterpret_cast<drm_version*>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		
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
		
		param->version_major = resp.drm_version_major();
		param->version_minor = resp.drm_version_minor();
		param->version_patchlevel = resp.drm_version_patchlevel();
		
		memcpy(param->name, resp.drm_driver_name().data(), frigg::min(param->name_len,
				resp.drm_driver_name().size()));
		memcpy(param->date, resp.drm_driver_date().data(), frigg::min(param->date_len,
				resp.drm_driver_date().size()));
		memcpy(param->desc, resp.drm_driver_desc().data(), frigg::min(param->desc_len,
				resp.drm_driver_desc().size()));

		param->name_len = resp.drm_driver_name().size();
		param->date_len = resp.drm_driver_date().size();
		param->desc_len = resp.drm_driver_desc().size();
		
		return resp.result();
	}
	case DRM_IOCTL_GET_CAP: {
		auto param = reinterpret_cast<drm_get_cap*>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_capability(param->capability);

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
		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			errno = EINVAL;
			return -1;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			
			param->value = resp.drm_value();
			return resp.result();
		}
	}
	case DRM_IOCTL_GET_MAGIC: {
		auto param = reinterpret_cast<drm_auth *>(arg);
		frigg::infoLogger() << "mlibc: DRM_IOCTL_GET_MAGIC is not implemented correctly"
				<< frigg::endLog;
		param->magic = 1;
		return 0;
	}
	case DRM_IOCTL_AUTH_MAGIC: {
		auto param = reinterpret_cast<drm_auth *>(arg);
		frigg::infoLogger() << "mlibc: DRM_IOCTL_AUTH_MAGIC is not implemented correctly"
				<< frigg::endLog;
		return 0;
	}
	case DRM_IOCTL_MODE_GETRESOURCES: {
		auto param = reinterpret_cast<drm_mode_card_res *>(arg);
		HelAction actions[3];
		globalQueue.trim();
		
		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		
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
		
		for(size_t i = 0; i < resp.drm_fb_ids_size(); i++) {
			if(i >= param->count_fbs)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->fb_id_ptr);
			dest[i] = resp.drm_fb_ids(i);
		}
		param->count_fbs = resp.drm_fb_ids_size();
		
		for(size_t i = 0; i < resp.drm_crtc_ids_size(); i++) {
			if(i >= param->count_crtcs)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->crtc_id_ptr);
			dest[i] = resp.drm_crtc_ids(i);
		}
		param->count_crtcs = resp.drm_crtc_ids_size();
		
		for(size_t i = 0; i < resp.drm_connector_ids_size(); i++) {
			if(i >= param->count_connectors)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->connector_id_ptr);
			dest[i] = resp.drm_connector_ids(i);
		}
		param->count_connectors = resp.drm_connector_ids_size();
		
		for(size_t i = 0; i < resp.drm_encoder_ids_size(); i++) {
			if(i >= param->count_encoders)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->encoder_id_ptr);
			dest[i] = resp.drm_encoder_ids(i);
		}
		param->count_encoders = resp.drm_encoder_ids_size();

		param->min_width = resp.drm_min_width();
		param->max_width = resp.drm_max_width();
		param->min_height = resp.drm_min_height();
		param->max_height = resp.drm_max_height();
		
		return resp.result();	
	}
	case DRM_IOCTL_MODE_GETCONNECTOR: {
		auto param = reinterpret_cast<drm_mode_get_connector*>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_connector_id(param->connector_id);

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
		actions[3].type = kHelActionRecvInline;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);
		auto recv_list = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);
		HEL_CHECK(recv_list->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		
		for(size_t i = 0; i < resp.drm_encoders_size(); i++) {
			if(i >= param->count_encoders)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->encoders_ptr);
			dest[i] = resp.drm_encoders(i);
		}
		param->count_encoders = resp.drm_encoders_size();
		
		for(size_t i = 0; i < resp.drm_num_modes(); i++) {
			if(i >= param->count_modes)
				 continue;
			auto dest = reinterpret_cast<drm_mode_modeinfo *>(param->modes_ptr);
			auto src = reinterpret_cast<drm_mode_modeinfo *>(recv_list->data);

			memcpy(&dest[i], &src[i], sizeof(drm_mode_modeinfo));
		}
		param->count_modes = resp.drm_num_modes();
		
		param->count_props = 0;
		param->encoder_id = resp.drm_encoder_id();
		param->connector_type = resp.drm_connector_type();
		param->connector_type_id = resp.drm_connector_type_id();
		param->connection = resp.drm_connection();
		param->mm_width = resp.drm_mm_width();
		param->mm_height = resp.drm_mm_height();
		param->subpixel = resp.drm_subpixel();
		param->pad = 0;
		
		return resp.result();
	}
	case DRM_IOCTL_MODE_GETENCODER: {
		auto param = reinterpret_cast<drm_mode_get_encoder*>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_encoder_id(param->encoder_id);

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

		param->encoder_type = resp.drm_encoder_type();
		param->crtc_id = resp.drm_crtc_id();
		param->possible_crtcs = resp.drm_possible_crtcs();
		param->possible_clones = resp.drm_possible_clones();
		
		return resp.result();
	}	
	case DRM_IOCTL_MODE_CREATE_DUMB: {
		auto param = reinterpret_cast<drm_mode_create_dumb*>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		
		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_bpp(param->bpp);
		req.set_drm_flags(param->flags);	

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
	
		param->handle = resp.drm_handle();
		param->pitch = resp.drm_pitch();
		param->size = resp.drm_size();
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_ADDFB: {
		auto param = reinterpret_cast<drm_mode_fb_cmd *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
	
		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitch);
		req.set_drm_bpp(param->bpp);
		req.set_drm_depth(param->depth);
		req.set_drm_handle(param->handle);
	
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

		param->fb_id = resp.drm_fb_id();
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_ADDFB2: {
		auto param = reinterpret_cast<drm_mode_fb_cmd2 *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		if(param->pixel_format != DRM_FORMAT_XRGB8888)
			frigg::infoLogger() << "mlibc: Unexpected pixel format "
					<< frigg::logHex(param->pixel_format) << frigg::endLog;
		__ensure(param->pixel_format == DRM_FORMAT_XRGB8888
				|| param->pixel_format == DRM_FORMAT_ARGB8888);
		__ensure(!param->flags);
		__ensure(!param->modifier[0]);
		__ensure(!param->offsets[0]);

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(DRM_IOCTL_MODE_ADDFB);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitches[0]);
		req.set_drm_bpp(32);
		req.set_drm_depth(24);
		req.set_drm_handle(param->handles[0]);
	
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

		param->fb_id = resp.drm_fb_id();
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_RMFB: {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
	
		req.set_drm_fb_id(*param);
	
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
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_MAP_DUMB: {
		auto param = reinterpret_cast<drm_mode_map_dumb*>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_handle(param->handle);
	
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
	
		param->offset = resp.drm_offset();
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_GETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_crtc_id(param->crtc_id);

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
		actions[3].buffer = &param->mode;
		actions[3].length = sizeof(drm_mode_modeinfo);
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);
		auto recv_data = parseLength(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);
		HEL_CHECK(recv_data->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fb_id = resp.drm_fb_id();
		param->x = resp.drm_x();
		param->y = resp.drm_y();
		param->gamma_size = resp.drm_gamma_size();
		param->mode_valid = resp.drm_mode_valid();

		return resp.result();
	}
	case DRM_IOCTL_MODE_SETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		for(size_t i = 0; i < param->count_connectors; i++) {
			auto dest = reinterpret_cast<uint32_t *>(param->set_connectors_ptr);
			req.add_drm_connector_ids(dest[i]);
		}
		req.set_drm_x(param->x);
		req.set_drm_y(param->y);
		req.set_drm_crtc_id(param->crtc_id);
		req.set_drm_fb_id(param->fb_id);
		req.set_drm_mode_valid(param->mode_valid);

		frigg::String<MemoryAllocator> ser(getAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionSendFromBuffer;
		actions[2].flags = kHelItemChain;
		actions[2].buffer = &param->mode;
		actions[2].length = sizeof(drm_mode_modeinfo);
		actions[3].type = kHelActionRecvInline;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseSimple(element);
		auto send_req = parseSimple(element);
		auto send_mode = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(send_mode->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		return resp.result();
	}
	case DRM_IOCTL_MODE_PAGE_FLIP: {
		auto param = reinterpret_cast<drm_mode_crtc_page_flip *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		assert(!(param->flags & ~DRM_MODE_PAGE_FLIP_EVENT));
		req.set_drm_crtc_id(param->crtc_id);
		req.set_drm_fb_id(param->fb_id);
		req.set_drm_cookie(param->user_data);

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

		return resp.result();
	}
	case DRM_IOCTL_MODE_DIRTYFB: {
		auto param = reinterpret_cast<drm_mode_fb_dirty_cmd*>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_fb_id(param->fb_id);
		req.set_drm_flags(param->flags);
		req.set_drm_color(param->color);
		for(size_t i = 0; i < param->num_clips; i++) {
			auto dest = reinterpret_cast<drm_clip_rect *>(param->clips_ptr);
			managarm::fs::Rect<MemoryAllocator> clip(getAllocator());
			clip.set_x1(dest->x1);
			clip.set_y1(dest->y1);
			clip.set_x2(dest->x2);
			clip.set_y2(dest->y2);
			req.add_drm_clips(frigg::move(clip));
		}
	
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
	
		return resp.result();
	}
	case DRM_IOCTL_MODE_CURSOR: {
		static bool infoPrinted = false;
		if(!infoPrinted) {
			frigg::infoLogger() << "mlibc: Cursor-specific DRM ioctl()s are not supported"
					<< frigg::endLog;
			infoPrinted = true;
		}
		auto param = reinterpret_cast<drm_mode_cursor *>(arg);
		if(param->handle)
			frigg::infoLogger() << "mlibc: DRM_IOCTL_MODE_CURSOR to ("
					<< param->x << ", " << param->y << ")" << frigg::endLog;
		errno = ENXIO;
		return -1;
	}
	} // end of switch()


	if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGVERSION)) {
		*reinterpret_cast<int *>(arg) = 0x010001; // should be EV_VERSION
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGID)) {
		memset(arg, 0, sizeof(struct input_id));
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGNAME(0))) {
		const char *s = "Managarm generic evdev";
		auto chunk = frigg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		return chunk;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPHYS(0))) {
		// Returns the sysfs path of the device.
		const char *s = "input0";
		auto chunk = frigg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		return chunk;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGUNIQ(0))) {
		// Returns a unique ID for the device.
		const char *s = "0";
		auto chunk = frigg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		return chunk;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPROP(0))) {
		// Returns a bitmask of properties of the device.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		return size;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGKEY(0))) {
		// Returns the current key state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		return size;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGLED(0))) {
		// Returns the current LED state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		return size;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGSW(0))) {
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		return size;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) >= _IOC_NR(EVIOCGBIT(0, 0))
			&& _IOC_NR(request) <= _IOC_NR(EVIOCGBIT(EV_MAX, 0))) {
		// Returns a bitmask of capabilities of the device.
		// If type is zero, return a mask of supported types.
		// As EV_SYN is zero, this implies that it is impossible
		// to get the mask of supported synthetic events.
		auto type = _IOC_NR(request) - _IOC_NR(EVIOCGBIT(0, 0));
		if(!type) {
			// TODO: Check with the Linux ABI if we have to do this.
			memset(arg, 0, _IOC_SIZE(request));

			HelAction actions[4];
			globalQueue.trim();

			managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(0, 0));
			req.set_size(_IOC_SIZE(request));

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
			actions[3].buffer = arg;
			actions[3].length = _IOC_SIZE(request);
			HEL_CHECK(helSubmitAsync(handle, actions, 4,
					globalQueue.getQueue(), 0, 0));

			auto element = globalQueue.dequeueSingle();
			auto offer = parseSimple(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			return recv_data->length;
		}else{
			// TODO: Check with the Linux ABI if we have to do this.
			memset(arg, 0, _IOC_SIZE(request));

			HelAction actions[4];
			globalQueue.trim();

			managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(1, 0));
			req.set_input_type(type);
			req.set_size(_IOC_SIZE(request));

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
			actions[3].buffer = arg;
			actions[3].length = _IOC_SIZE(request);
			HEL_CHECK(helSubmitAsync(handle, actions, 4,
					globalQueue.getQueue(), 0, 0));

			auto element = globalQueue.dequeueSingle();
			auto offer = parseSimple(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			return recv_data->length;
		}
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOSCLOCKID)) {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_input_clock(*param);
	
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
		return resp.result();
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) >= _IOC_NR(EVIOCGABS(0))
			&& _IOC_NR(request) <= _IOC_NR(EVIOCGABS(ABS_MAX))) {
		frigg::infoLogger() << "mlibc: EVIOCGABS is not implemented correctly"
				<< frigg::endLog;
		return -1;
	}
	
	frigg::infoLogger() << "mlibc: Unexpected ioctl with"
			<< " type: 0x" << frigg::logHex(_IOC_TYPE(request))
			<< ", number: 0x" << frigg::logHex(_IOC_NR(request))
			<< " (raw request: " << frigg::logHex(request) << ")" << frigg::endLog;
	__ensure(!"Illegal ioctl request");
	__builtin_unreachable();
}

int sys_open(const char *path, int flags, int *fd) {
	HelAction actions[3];

	globalQueue.trim();

	uint32_t proto_flags = 0;
	frigg::infoLogger() << "mlibc: flags: " << frigg::logHex(flags) << frigg::endLog;
	if(flags & __MLIBC_O_CREAT)
		proto_flags |= managarm::posix::OpenFlags::OF_CREATE;
	if(flags & __MLIBC_O_EXCL)
		proto_flags |= managarm::posix::OpenFlags::OF_EXCLUSIVE;
	if(flags & __MLIBC_O_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;

	if(flags & __MLIBC_O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::OPEN);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));
	req.set_flags(proto_flags);

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
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*fd = resp.fd();
		return 0;
	}
}

int sys_read(int fd, void *data, size_t max_size, ssize_t *bytes_read) {
	//frigg::infoLogger() << "read() " << max_size << frigg::EndLog();
	HelAction actions[5];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);

	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::READ);
	req.set_fd(fd);
	req.set_size(max_size);

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
	actions[3].type = kHelActionRecvInline;
	actions[3].flags = kHelItemChain;
	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = 0;
	actions[4].buffer = data;
	actions[4].length = max_size;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
/*	if(resp.error() == managarm::fs::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else*/
	if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		errno = EINVAL;
		return -1;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		errno = EAGAIN;
		return -1;
	}else if(resp.error() == managarm::fs::Errors::END_OF_FILE) {
		*bytes_read = 0;
		return 0;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		HEL_CHECK(recv_data->error);
		*bytes_read = recv_data->length;
		return 0;
	}
}

int sys_write(int fd, const void *data, size_t size, ssize_t *bytes_written) {
	HelAction actions[5];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);

//	frigg::infoLogger.log() << "write()" << frigg::EndLog();
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::WRITE);
	req.set_fd(fd);

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
	actions[3].buffer = const_cast<void *>(data);
	actions[3].length = size;
	actions[4].type = kHelActionRecvInline;
	actions[4].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto send_data = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(send_data->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

	// TODO: implement NO_SUCH_FD
/*	if(resp.error() == managarm::fs::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else*/ if(resp.error() == managarm::fs::Errors::SUCCESS) {
		//FIXME: handle partial writes
		*bytes_written = size;
		return 0;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	HelAction actions[3];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);
	
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_fd(fd);
	req.set_rel_offset(offset);

	if(whence == SEEK_SET) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_ABS);
	}else if(whence == SEEK_CUR) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_REL);
	}else if(whence == SEEK_END) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_EOF);
	}else{
		frigg::panicLogger() << "Illegal whence argument" << frigg::endLog;
	}
	
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
	*new_offset = resp.offset();
	return 0;
}


int sys_close(int fd) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::CLOSE);
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
	
	if(resp.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;		
		return -1;
	}else if(resp.error() == managarm::posix::Errors::SUCCESS) {
		return 0;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

int sys_dup(int fd, int flags, int *newfd) {
	HelAction actions[3];
	globalQueue.trim();
	
	assert(!(flags & ~(__MLIBC_O_CLOEXEC)));
	
	uint32_t proto_flags = 0;
	if(flags & __MLIBC_O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::DUP);
	req.set_fd(fd);
	req.set_flags(proto_flags);

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

int sys_dup2(int fd, int flags, int newfd) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::DUP2);
	req.set_fd(fd);
	req.set_newfd(newfd);
	req.set_flags(flags);

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
	return 0;
}

int sys_stat(const char *path, struct stat *result) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::STAT);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		memset(result, 0, sizeof(struct stat));
		
		switch(resp.file_type()) {
		case managarm::posix::FileType::FT_REGULAR:
			result->st_mode = S_IFREG; break;
		case managarm::posix::FileType::FT_DIRECTORY:
			result->st_mode = S_IFDIR; break;
		case managarm::posix::FileType::FT_CHAR_DEVICE:
			result->st_mode = S_IFCHR; break;
		case managarm::posix::FileType::FT_BLOCK_DEVICE:
			result->st_mode = S_IFBLK; break;
		}

		result->st_dev = 1;
		result->st_ino = resp.fs_inode();
		result->st_mode |= resp.mode();
		result->st_nlink = resp.num_links();
		result->st_uid = resp.uid();
		result->st_gid = resp.gid();
		result->st_rdev = resp.ref_devnum();
		result->st_size = resp.file_size();
		result->st_atim.tv_sec = resp.atime_secs();
		result->st_atim.tv_nsec = resp.atime_nanos();
		result->st_mtim.tv_sec = resp.mtime_secs();
		result->st_mtim.tv_nsec = resp.mtime_nanos();
		result->st_ctim.tv_sec = resp.ctime_secs();
		result->st_ctim.tv_nsec = resp.ctime_nanos();
		result->st_blksize = 4096;
		result->st_blocks = resp.file_size() / 512 + 1;
		return 0;
	}
}

int sys_fstat(int fd, struct stat *result) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::FSTAT);
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
	
	memset(result, 0, sizeof(struct stat));
		
	switch(resp.file_type()) {
	case managarm::posix::FileType::FT_REGULAR:
		result->st_mode = S_IFREG; break;
	case managarm::posix::FileType::FT_DIRECTORY:
		result->st_mode = S_IFDIR; break;
	case managarm::posix::FileType::FT_CHAR_DEVICE:
		result->st_mode = S_IFCHR; break;
	case managarm::posix::FileType::FT_BLOCK_DEVICE:
		result->st_mode = S_IFBLK; break;
	}

	result->st_dev = 1;
	result->st_ino = resp.fs_inode();
	result->st_mode |= resp.mode();
	result->st_nlink = resp.num_links();
	result->st_uid = resp.uid();
	result->st_gid = resp.gid();
	result->st_rdev = resp.ref_devnum();
	result->st_size = resp.file_size();
	result->st_atim.tv_sec = resp.atime_secs();
	result->st_atim.tv_nsec = resp.atime_nanos();
	result->st_mtim.tv_sec = resp.mtime_secs();
	result->st_mtim.tv_nsec = resp.mtime_nanos();
	result->st_ctim.tv_sec = resp.ctime_secs();
	result->st_ctim.tv_nsec = resp.ctime_nanos();
	result->st_blksize = 4096;
	result->st_blocks = resp.file_size() / 512 + 1;
	return 0;
}

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	HelAction actions[4];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::READLINK);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	actions[3].buffer = data;
	actions[3].length = max_size;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseSimple(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_data->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		errno = EINVAL;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*length = recv_data->length;
		return 0;
	}
}

int sys_ftruncate(int fd, size_t size) {
	HelAction actions[3];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);
	
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_TRUNCATE);
	req.set_size(size);

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
}

int sys_fallocate(int fd, off_t offset, size_t size) {
	HelAction actions[3];
	globalQueue.trim();

	auto handle = cacheFileTable()[fd];
	__ensure(handle);
	
	managarm::fs::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_FALLOCATE);
	req.set_rel_offset(offset);
	req.set_size(size);

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
}

int sys_unlink(const char *path) {
	HelAction actions[3];

	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::UNLINK);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_access(const char *path, int mode) {
	HelAction actions[3];

	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::ACCESS);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), path));

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
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_isatty(int fd, int *ptr) {
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::IS_TTY);
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
	if(resp.error() ==  managarm::posix::Errors::BAD_FD) {
		errno = ENOTTY;
		*ptr = 0;
		return -1;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*ptr = 1;
		return 0;
	}
}

} //namespace mlibc

