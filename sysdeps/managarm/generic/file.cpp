
#include <string.h>
#include <errno.h>

#include <sys/auxv.h>

#include <asm/ioctls.h>
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
// for EFD_CLOEXEC, EFD_NONBLOCK, EFD_SEMAPHONE
#include <sys/eventfd.h>
// for ioctl()
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <sys/sysmacros.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <linux/cdrom.h>
#include <linux/vt.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <posix.frigg_bragi.hpp>
#include <fs.frigg_bragi.hpp>

HelHandle __mlibc_getPassthrough(int fd) {
	auto handle = getHandleForFd(fd);
	__ensure(handle);
	return handle;
}

namespace mlibc {

int sys_chdir(const char *path) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::CHDIR);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_fchdir(int fd) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::FCHDIR);
	req.set_fd(fd);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
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

int sys_chroot(const char *path) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::CHROOT);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
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

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	(void)mode;
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::MKDIRAT);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_fd(dirfd);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		return EEXIST;
	} else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::NOT_A_DIRECTORY) {
		return ENOTDIR;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_symlink(const char *target_path, const char *link_path) {
	return sys_symlinkat(target_path, AT_FDCWD, link_path);
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
	SignalGuard sguard;

	managarm::posix::SymlinkAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), link_path));
	req.set_target_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), target_path));

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	SignalGuard sguard;

	managarm::posix::LinkAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), old_path));
	req.set_target_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), new_path));
	req.set_fd(olddirfd);
	req.set_newfd(newdirfd);
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_rename(const char *path, const char *new_path) {
	return sys_renameat(AT_FDCWD, path, AT_FDCWD, new_path);
}

int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	SignalGuard sguard;

	managarm::posix::RenameAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), old_path));
	req.set_target_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), new_path));
	req.set_fd(olddirfd);
	req.set_newfd(newdirfd);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

} //namespace mlibc

HelHandle __raw_map(int fd) {
	SignalGuard sguard;
	HelAction actions[4];
	globalQueue.trim();

	auto handle = getHandleForFd(fd);
	if (!handle)
		return 0;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::MMAP);
	req.set_fd(fd);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto pull_memory = parseHandle(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(pull_memory->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
	return pull_memory->handle;
}

namespace mlibc {

int sys_fcntl(int fd, int request, va_list args, int *result) {
	SignalGuard sguard;
	if(request == F_DUPFD) {
		int newfd;
		if(int e = sys_dup(fd, 0, &newfd); e)
			return e;
		*result = newfd;
		return 0;
	}else if(request == F_DUPFD_CLOEXEC) {
		int newfd;
		if(int e = sys_dup(fd, O_CLOEXEC, &newfd); e)
			return e;
		*result = newfd;
		return 0;
	}else if(request == F_GETFD) {
		HelAction actions[3];
		globalQueue.trim();

		managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_request_type(managarm::posix::CntReqType::FD_GET_FLAGS);
		req.set_fd(fd);

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
		HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		if(resp.error() == managarm::posix::Errors::NO_SUCH_FD)
			return EBADF;
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*result = resp.flags();
		return 0;
	}else if(request == F_SETFD) {
		HelAction actions[3];
		globalQueue.trim();

		managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_request_type(managarm::posix::CntReqType::FD_SET_FLAGS);
		req.set_fd(fd);
		req.set_flags(va_arg(args, int));

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
		HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		if(resp.error() == managarm::posix::Errors::NO_SUCH_FD)
			return EBADF;
		else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS)
			return EINVAL;
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*result = static_cast<int>(resp.error());
		return 0;
	}else if(request == F_GETFL) {
		SignalGuard sguard;
		HelAction actions[3];
		globalQueue.trim();

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_FILE_FLAGS);
		req.set_fd(fd);

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
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_GETFL) unimplemented for this file\e[39m" << frg::endlog;
			return EINVAL;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.flags();
		return 0;
	}else if(request == F_SETFL) {
		SignalGuard sguard;
		HelAction actions[3];
		globalQueue.trim();

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_SET_FILE_FLAGS);
		req.set_fd(fd);
		req.set_flags(va_arg(args, int));

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
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_SETFL) unimplemented for this file\e[39m" << frg::endlog;
			return EINVAL;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = 0;
		return 0;
	}else if(request == F_SETLK) {
		mlibc::infoLogger() << "\e[31mmlibc: F_SETLK\e[39m" << frg::endlog;
		return 0;
	}else if(request == F_SETLKW) {
		mlibc::infoLogger() << "\e[31mmlibc: F_SETLKW\e[39m" << frg::endlog;
		return 0;
	}else if(request == F_GETLK) {
		mlibc::infoLogger() << "\e[31mmlibc: F_GETLK\e[39m" << frg::endlog;
		return ENOSYS;
	}else if(request == F_ADD_SEALS) {
		auto seals = va_arg(args, int);
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_ADD_SEALS);
		req.set_fd(fd);
		req.set_seals(seals);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::RecvInline()
			));

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_ADD_SEALS) unimplemented for this file\e[39m" << frg::endlog;
			return EINVAL;
		} else if(resp.error() == managarm::fs::Errors::INSUFFICIENT_PERMISSIONS) {
			return EPERM;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.seals();
		return 0;
	}else if(request == F_GET_SEALS) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_SEALS);
		req.set_fd(fd);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::RecvInline()
			));

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_GET_SEALS) unimplemented for this file\e[39m" << frg::endlog;
			return EINVAL;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.seals();
		return 0;
	}else{
		mlibc::infoLogger() << "\e[31mmlibc: Unexpected fcntl() request: "
				<< request << "\e[39m" << frg::endlog;
		return EINVAL;
	}
}

int sys_open_dir(const char *path, int *handle) {
	return sys_open(path, 0, 0, handle);
}

int sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read) {
	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	HelAction actions[3];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_READ_ENTRIES);

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
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::TTY_NAME);
	req.set_fd(fd);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() ==  managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::NOT_A_TTY) {
		return ENOTTY;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		__ensure(size >= resp.path().size() + 1);
		memcpy(buf, resp.path().data(), size);
		buf[resp.path().size()] = '\0';
		return 0;
	}
}

int sys_fdatasync(int) {
	mlibc::infoLogger() << "\e[35mmlibc: fdatasync() is a no-op\e[39m"
			<< frg::endlog;
	return 0;
}

int sys_getcwd(char *buffer, size_t size) {
	SignalGuard sguard;
	HelAction actions[4];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::GETCWD);
	req.set_size(size);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
	actions[3].buffer = buffer;
	actions[3].length = size;
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_path = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_path->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	if(static_cast<size_t>(resp.size()) >= size)
		return ERANGE;
	return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	SignalGuard sguard;

	managarm::posix::VmMapRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_address_hint(reinterpret_cast<uintptr_t>(hint));
	req.set_size(size);
	req.set_mode(prot);
	req.set_flags(flags);
	req.set_fd(fd);
	req.set_rel_offset(offset);

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
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_REMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_new_size(new_size);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_PROTECT);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_mode(prot);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
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

int sys_vm_unmap(void *pointer, size_t size) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_UNMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
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

int sys_setsid(pid_t *sid) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SETSID);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::ACCESS_DENIED) {
		*sid = -1;
		return EPERM;
	}
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*sid = resp.sid();
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	int result;
	if(int e = sys_ioctl(fd, TCGETS, attr, &result); e)
		return e;
	return 0;
}

int sys_tcsetattr(int fd, int when, const struct termios *attr) {
	if(when != TCSANOW)
		mlibc::infoLogger() << "\e[35mmlibc: tcsetattr() when argument ignored\e[39m"
				<< frg::endlog;
	int result;
	if(int e = sys_ioctl(fd, TCSETS, const_cast<struct termios *>(attr), &result); e)
		return e;
	return 0;
}

int sys_tcdrain(int) {
	mlibc::infoLogger() << "\e[35mmlibc: tcdrain() is a stub\e[39m" << frg::endlog;
	return 0;
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
	constexpr int type_mask = int(0xFFFF);
	constexpr int flags_mask = ~int(0xFFFF);
	__ensure(!((type_and_flags & flags_mask) & ~(SOCK_CLOEXEC | SOCK_NONBLOCK)));

	SignalGuard sguard;

	managarm::posix::SocketRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_domain(domain);
	req.set_socktype(type_and_flags & type_mask);
	req.set_protocol(proto);
	req.set_flags(type_and_flags & flags_mask);

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
	if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EAFNOSUPPORT;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*fd = resp.fd();
		return 0;
	}
}

int sys_pipe(int *fds, int flags) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::PIPE_CREATE);
	req.set_flags(flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	__ensure(resp.fds_size() == 2);
	fds[0] = resp.fds(0);
	fds[1] = resp.fds(1);
	return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	constexpr int type_mask = int(0xFFFF);
	constexpr int flags_mask = ~int(0xFFFF);
	__ensure(!((type_and_flags & flags_mask) & ~(SOCK_CLOEXEC | SOCK_NONBLOCK)));

	SignalGuard sguard;

	managarm::posix::SockpairRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_domain(domain);
	req.set_socktype(type_and_flags & type_mask);
	req.set_protocol(proto);
	req.set_flags(type_and_flags & flags_mask);

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
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	__ensure(resp.fds_size() == 2);
	fds[0] = resp.fds(0);
	fds[1] = resp.fds(1);
	return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length) {
	HelSgItem sglist[4];
	__ensure(hdr->msg_iovlen <= 4);
	auto handle = getHandleForFd(sockfd);
	if (!handle)
		return EBADF;

	size_t overall_size = 0;
	for(int i = 0; i < hdr->msg_iovlen; i++) {
		sglist[i].buffer = hdr->msg_iov[i].iov_base;
		sglist[i].length = hdr->msg_iov[i].iov_len;
		overall_size += hdr->msg_iov[i].iov_len;
	}

	SignalGuard sguard;
	HelAction actions[6];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_SENDMSG);
	req.set_flags(flags);
	req.set_size(overall_size);

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

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;

	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();

	actions[2].type = kHelActionSendFromBufferSg;
	actions[2].flags = kHelItemChain;
	actions[2].buffer = &sglist;
	actions[2].length = hdr->msg_iovlen;

	actions[3].type = kHelActionImbueCredentials;
	actions[3].handle = kHelThisThread;
	actions[3].flags = kHelItemChain;

	actions[4].type = kHelActionSendFromBuffer;
	actions[4].flags = kHelItemChain;
	actions[4].buffer = hdr->msg_name;
	actions[4].length = hdr->msg_namelen;

	actions[5].type = kHelActionRecvInline;
	actions[5].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 6,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto send_data = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto send_addr = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(send_data->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(send_addr->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	if(resp.error() == managarm::fs::Errors::BROKEN_PIPE) {
		return EPIPE;
	}else if(resp.error() == managarm::fs::Errors::NOT_CONNECTED) {
		return ENOTCONN;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
	}else if(resp.error() == managarm::fs::Errors::HOST_UNREACHABLE) {
		return EHOSTUNREACH;
	}else if(resp.error() == managarm::fs::Errors::ACCESS_DENIED) {
		return EACCES;
	}else if(resp.error() == managarm::fs::Errors::NETWORK_UNREACHABLE) {
		return ENETUNREACH;
	}else if(resp.error() == managarm::fs::Errors::DESTINATION_ADDRESS_REQUIRED) {
		return EDESTADDRREQ;
	}else if(resp.error() == managarm::fs::Errors::ADDRESS_NOT_AVAILABLE) {
		return EADDRNOTAVAIL;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*length = resp.size();
		return 0;
	}
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
	if(!hdr->msg_iovlen) {
		return EMSGSIZE;
	}

	auto handle = getHandleForFd(sockfd);
	if (!handle)
		return EBADF;

	SignalGuard sguard;
	HelAction actions[7];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_RECVMSG);
	req.set_flags(flags);
	req.set_size(hdr->msg_iov[0].iov_len);
	req.set_addr_size(hdr->msg_namelen);
	req.set_ctrl_size(hdr->msg_controllen);

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

	actions[3].type = kHelActionRecvInline;
	actions[3].flags = kHelItemChain;

	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = kHelItemChain;
	actions[4].buffer = hdr->msg_name;
	actions[4].length = hdr->msg_namelen;

	actions[5].type = kHelActionRecvToBuffer;
	actions[5].flags = kHelItemChain;
	actions[5].buffer = hdr->msg_iov[0].iov_base;
	actions[5].length = hdr->msg_iov[0].iov_len;

	actions[6].type = kHelActionRecvToBuffer;
	actions[6].flags = 0;
	actions[6].buffer = hdr->msg_control;
	actions[6].length = hdr->msg_controllen;
	HEL_CHECK(helSubmitAsync(handle, actions, 7,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_addr = parseLength(element);
	auto recv_data = parseLength(element);
	auto recv_ctrl = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		HEL_CHECK(recv_addr->error);
		HEL_CHECK(recv_data->error);
		HEL_CHECK(recv_ctrl->error);

		hdr->msg_namelen = resp.addr_size();
		hdr->msg_controllen = recv_ctrl->length;
		*length = recv_data->length;
		return 0;
	}
}

int sys_pselect(int, fd_set *read_set, fd_set *write_set,
		fd_set *except_set, const struct timespec *timeout,
		const sigset_t *sigmask, int *num_events) {
	// TODO: Do not keep errors from epoll (?).
	int fd = epoll_create1(0);
	if(fd == -1)
		return -1;

	for(int k = 0; k < FD_SETSIZE; k++) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));

		if(read_set && FD_ISSET(k, read_set))
			ev.events |= EPOLLIN; // TODO: Additional events.
		if(write_set && FD_ISSET(k, write_set))
			ev.events |= EPOLLOUT; // TODO: Additional events.
		if(except_set && FD_ISSET(k, except_set))
			ev.events |= EPOLLPRI;

		if(!ev.events)
			continue;
		ev.data.u32 = k;

		if(epoll_ctl(fd, EPOLL_CTL_ADD, k, &ev))
			return -1;
	}

	struct epoll_event evnts[16];
	int n = epoll_pwait(fd, evnts, 16,
		timeout ? (timeout->tv_sec * 1000 + timeout->tv_nsec / 100) : -1, sigmask);
	if(n == -1)
		return -1;

	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);
	int m = 0;

	for(int i = 0; i < n; i++) {
		int k = evnts[i].data.u32;

		if(read_set && FD_ISSET(k, read_set)
				&& evnts[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_read_set);
			m++;
		}

		if(write_set && FD_ISSET(k, write_set)
				&& evnts[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_write_set);
			m++;
		}

		if(except_set && FD_ISSET(k, except_set)
				&& evnts[i].events & EPOLLPRI) {
			FD_SET(k, &res_except_set);
			m++;
		}
	}

	if(close(fd))
		__ensure("close() failed on epoll file");

	if(read_set)
		memcpy(read_set, &res_read_set, sizeof(fd_set));
	if(write_set)
		memcpy(write_set, &res_write_set, sizeof(fd_set));
	if(except_set)
		memcpy(except_set, &res_except_set, sizeof(fd_set));

	*num_events = m;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	__ensure(timeout >= 0 || timeout == -1); // TODO: Report errors correctly.

	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_CALL);
	req.set_timeout(timeout > 0 ? int64_t{timeout} * 1000000 : timeout);

	for(nfds_t i = 0; i < count; i++) {
		req.add_fds(fds[i].fd);
		req.add_events(fds[i].events);
	}

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	__ensure(resp.events_size() == count);

	int m = 0;
	for(nfds_t i = 0; i < count; i++) {
		if(resp.events(i))
			m++;
		fds[i].revents = resp.events(i);
	}

	*num_events = m;
	return 0;
}

int sys_epoll_create(int flags, int *fd) {
	__ensure(!(flags & ~(EPOLL_CLOEXEC)));

	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	uint32_t proto_flags = 0;
	if(flags & EPOLL_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_CREATE);
	req.set_flags(proto_flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
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
		req.set_request_type(managarm::posix::CntReqType::EPOLL_DELETE);
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Illegal epoll_ctl() mode\e[39m" << frg::endlog;
	}
	req.set_fd(epfd);
	req.set_newfd(fd);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	} else if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		return EEXIST;
	} else if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
	return 0;
}

int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n,
		int timeout, const sigset_t *sigmask, int *raised) {
	__ensure(timeout >= 0 || timeout == -1); // TODO: Report errors correctly.

	SignalGuard sguard;
	HelAction actions[4];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_WAIT);
	req.set_fd(epfd);
	req.set_size(n);
	req.set_timeout(timeout > 0 ? int64_t{timeout} * 1000000 : timeout);
	if(sigmask != NULL) {
		req.set_sigmask((long int)*sigmask);
		req.set_sigmask_needed(true);
	} else {
		req.set_sigmask_needed(false);
	}

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
	actions[3].buffer = ev;
	actions[3].length = n * sizeof(struct epoll_event);
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_data->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	__ensure(!(recv_data->length % sizeof(struct epoll_event)));
	*raised = recv_data->length / sizeof(struct epoll_event);
	return 0;
}

int sys_timerfd_create(int clockid, int flags, int *fd) {
	(void) clockid;
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::TIMERFD_CREATE);
	req.set_flags(flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}

int sys_timerfd_settime(int fd, int,
		const struct itimerspec *value, struct itimerspec *oldvalue) {
	__ensure(!oldvalue);
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::TIMERFD_SETTIME);
	req.set_fd(fd);
	req.set_time_secs(value->it_value.tv_sec);
	req.set_time_nanos(value->it_value.tv_nsec);
	req.set_interval_secs(value->it_interval.tv_sec);
	req.set_interval_nanos(value->it_interval.tv_nsec);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
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

int sys_signalfd_create(const sigset_t *masks, int flags, int *fd)  {
	__ensure(!(flags & ~(SFD_CLOEXEC | SFD_NONBLOCK)));

	uint32_t proto_flags = 0;
	if(flags & SFD_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if(flags & SFD_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;

	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SIGNALFD_CREATE);
	req.set_flags(proto_flags);
	req.set_sigset(*masks);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}

int sys_inotify_create(int flags, int *fd) {
	__ensure(!(flags & ~(IN_CLOEXEC)));

	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if(flags & IN_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::InotifyCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(proto_flags);

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
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}

int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd) {
	SignalGuard sguard;

	managarm::posix::InotifyAddRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(ifd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(mask);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*wd = resp.wd();
		return 0;
	}
}

int sys_eventfd_create(unsigned int initval, int flags, int *fd) {
	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if (flags & EFD_NONBLOCK) proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;
	if (flags & EFD_CLOEXEC) proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if (flags & EFD_SEMAPHORE)
		return ENOSYS;

	managarm::posix::EventfdCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(proto_flags);
	req.set_initval(initval);

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
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	*fd = resp.fd();
	return 0;
}


int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
//	mlibc::infoLogger() << "mlibc: ioctl with"
//			<< " type: 0x" << frg::hex_fmt(_IOC_TYPE(request))
//			<< ", number: 0x" << frg::hex_fmt(_IOC_NR(request))
//			<< " (raw request: " << frg::hex_fmt(request) << ")"
//			<< " on fd " << fd << frg::endlog;

	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if(!handle)
		return EBADF;

	switch(request) {
	case FIONBIO: {
		auto mode = reinterpret_cast<int *>(arg);
		int flags = fcntl(fd, F_GETFL, 0);
		if(*mode) {
		    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
		}else{
		    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
		}
		return 0;
	}
	case FIONREAD: {
		auto argp = reinterpret_cast<int *>(arg);

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		if(!argp)
			return EINVAL;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(FIONREAD);

		auto [offer, send_req, recv_resp] =
		exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*argp = resp.fionread_count();

		return 0;
	}
	case FIOCLEX: {
		managarm::posix::IoctlFioclexRequest<MemoryAllocator> req(getSysdepsAllocator());
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
		if(recvResp.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recvResp.error());

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recvResp.data(), recvResp.length());
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
	case DRM_IOCTL_VERSION: {
		auto param = reinterpret_cast<drm_version*>(arg);

		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

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

		param->version_major = resp.drm_version_major();
		param->version_minor = resp.drm_version_minor();
		param->version_patchlevel = resp.drm_version_patchlevel();

		if(param->name)
			memcpy(param->name, resp.drm_driver_name().data(), frg::min(param->name_len,
					resp.drm_driver_name().size()));
		if(param->date)
			memcpy(param->date, resp.drm_driver_date().data(), frg::min(param->date_len,
					resp.drm_driver_date().size()));
		if(param->desc)
			memcpy(param->desc, resp.drm_driver_desc().data(), frg::min(param->desc_len,
					resp.drm_driver_desc().size()));

		param->name_len = resp.drm_driver_name().size();
		param->date_len = resp.drm_driver_date().size();
		param->desc_len = resp.drm_driver_desc().size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_GET_CAP: {
		auto param = reinterpret_cast<drm_get_cap*>(arg);

		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_capability(param->capability);

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

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->value = resp.drm_value();
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_SET_CLIENT_CAP: {
		auto param = reinterpret_cast<drm_set_client_cap *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_SET_CLIENT_CAP(" << param->capability << ") ignores its value\e[39m" << frg::endlog;

		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_capability(param->capability);

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

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->value = resp.drm_value();
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_GET_MAGIC: {
		auto param = reinterpret_cast<drm_auth *>(arg);
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_GET_MAGIC is not implemented correctly\e[39m"
				<< frg::endlog;
		param->magic = 1;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_AUTH_MAGIC: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_AUTH_MAGIC is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_SET_MASTER: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_SET_MASTER is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_DROP_MASTER: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_DROP_MASTER is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_GETRESOURCES: {
		auto param = reinterpret_cast<drm_mode_card_res *>(arg);

		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		if(recv_resp.error() == kHelErrDismissed) {
			return EINVAL;
		}

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETCONNECTOR: {
		auto param = reinterpret_cast<drm_mode_get_connector*>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_connector_id(param->connector_id);
		req.set_drm_max_modes(param->count_modes);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
		actions[3].buffer = reinterpret_cast<drm_mode_modeinfo *>(param->modes_ptr);
		actions[3].length = param->count_modes * sizeof(drm_mode_modeinfo);
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);
		auto recv_list = parseLength(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);
		HEL_CHECK(recv_list->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		for(size_t i = 0; i < resp.drm_encoders_size(); i++) {
			if(i >= param->count_encoders)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->encoders_ptr);
			dest[i] = resp.drm_encoders(i);
		}

		param->count_props = 0;
		param->encoder_id = resp.drm_encoder_id();
		param->connector_type = resp.drm_connector_type();
		param->connector_type_id = resp.drm_connector_type_id();
		param->connection = resp.drm_connection();
		param->mm_width = resp.drm_mm_width();
		param->mm_height = resp.drm_mm_height();
		param->subpixel = resp.drm_subpixel();
		param->pad = 0;
		param->count_encoders = resp.drm_encoders_size();
		param->count_modes = resp.drm_num_modes();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPROPERTY: {
		auto param = reinterpret_cast<drm_mode_get_property*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_property_id(param->prop_id);

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

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPERTY(" << param->prop_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		memcpy(param->name, resp.drm_property_name().data(), resp.drm_property_name().size());
		param->count_values = resp.drm_property_vals_size();
		param->flags = resp.drm_property_flags();

		for(size_t i = 0; i < param->count_values && i < resp.drm_property_vals_size() && param->values_ptr; i++) {
			auto dest = reinterpret_cast<uint64_t *>(param->values_ptr);
			dest[i] = resp.drm_property_vals(i);
		}

		__ensure(resp.drm_enum_name_size() == resp.drm_enum_value_size());

		for(size_t i = 0; i < param->count_enum_blobs && i < resp.drm_enum_name_size() && i < resp.drm_enum_value_size(); i++) {
			auto dest = reinterpret_cast<drm_mode_property_enum *>(param->enum_blob_ptr);
			dest[i].value = resp.drm_enum_value(i);
			strncpy(dest[i].name, resp.drm_enum_name(i).data(), DRM_PROP_NAME_LEN);
		}

		param->count_enum_blobs = resp.drm_enum_name_size();

		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_SETPROPERTY: {
		auto param = reinterpret_cast<drm_mode_connector_set_property *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_property_id(param->prop_id);
		req.set_drm_property_value(param->value);
		req.set_drm_obj_id(param->connector_id);

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

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_SETPROPERTY(" << param->prop_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_get_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_id(param->blob_id);

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

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPBLOB(" << param->blob_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		uint8_t *dest = reinterpret_cast<uint8_t *>(param->data);
		for(size_t i = 0; i < resp.drm_property_blob_size(); i++) {
			if(i >= param->length) {
				continue;
			}

			dest[i] = resp.drm_property_blob(i);
		}

		param->length = resp.drm_property_blob_size();

		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_GETPLANE: {
		auto param = reinterpret_cast<drm_mode_get_plane*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_plane_id(param->plane_id);

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

		param->crtc_id = resp.drm_crtc_id();
		param->fb_id = resp.drm_fb_id();
		param->possible_crtcs = resp.drm_possible_crtcs();
		param->gamma_size = resp.drm_gamma_size();

		// FIXME: this should be passed as a buffer with helix, but this has no bounded max size?
		for(size_t i = 0; i < resp.drm_format_type_size(); i+= 4) {
			if(i >= param->count_format_types) {
				break;
			}
			auto dest = reinterpret_cast<uint32_t *>(param->format_type_ptr);
			dest[i] = resp.drm_format_type(i);
		}

		param->count_format_types = resp.drm_format_type_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPLANERESOURCES: {
		auto param = reinterpret_cast<drm_mode_get_plane_res *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBuffer(ser.data(), ser.size()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		// FIXME: send this via a helix_ng buffer
		for(size_t i = 0; i < resp.drm_plane_res_size(); i++) {
			if(i >= param->count_planes) {
				continue;
			}
			auto dest = reinterpret_cast<uint32_t *>(param->plane_id_ptr);
			dest[i] = resp.drm_plane_res(i);
		}

		param->count_planes = resp.drm_plane_res_size();

		*result = resp.result();

		return 0;
	}
	case DRM_IOCTL_MODE_GETENCODER: {
		auto param = reinterpret_cast<drm_mode_get_encoder*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_encoder_id(param->encoder_id);

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

		param->encoder_type = resp.drm_encoder_type();
		param->crtc_id = resp.drm_crtc_id();
		param->possible_crtcs = resp.drm_possible_crtcs();
		param->possible_clones = resp.drm_possible_clones();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_CREATE_DUMB: {
		auto param = reinterpret_cast<drm_mode_create_dumb*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_bpp(param->bpp);
		req.set_drm_flags(param->flags);

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

		param->handle = resp.drm_handle();
		param->pitch = resp.drm_pitch();
		param->size = resp.drm_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ADDFB: {
		auto param = reinterpret_cast<drm_mode_fb_cmd *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitch);
		req.set_drm_bpp(param->bpp);
		req.set_drm_depth(param->depth);
		req.set_drm_handle(param->handle);

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

		param->fb_id = resp.drm_fb_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ADDFB2: {
		auto param = reinterpret_cast<drm_mode_fb_cmd2 *>(arg);

		if(param->pixel_format != DRM_FORMAT_XRGB8888)
			mlibc::infoLogger() << "mlibc: Unexpected pixel format "
					<< frg::hex_fmt(param->pixel_format) << frg::endlog;
		__ensure(param->pixel_format == DRM_FORMAT_XRGB8888
				|| param->pixel_format == DRM_FORMAT_ARGB8888);
		__ensure(!param->flags);
		__ensure(!param->modifier[0]);
		__ensure(!param->offsets[0]);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(DRM_IOCTL_MODE_ADDFB);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitches[0]);
		req.set_drm_bpp(32);
		req.set_drm_depth(24);
		req.set_drm_handle(param->handles[0]);

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

		param->fb_id = resp.drm_fb_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_RMFB: {
		auto param = reinterpret_cast<int *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_fb_id(*param);

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_MAP_DUMB: {
		auto param = reinterpret_cast<drm_mode_map_dumb*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_handle(param->handle);

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

		param->offset = resp.drm_offset();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_crtc_id(param->crtc_id);

		auto [offer, send_req, recv_resp, recv_data] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline(),
				helix_ng::recvBuffer(&param->mode, sizeof(drm_mode_modeinfo)))
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());
		HEL_CHECK(recv_data.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fb_id = resp.drm_fb_id();
		param->x = resp.drm_x();
		param->y = resp.drm_y();
		param->gamma_size = resp.drm_gamma_size();
		param->mode_valid = resp.drm_mode_valid();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_SETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
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

		auto [offer, send_req, send_mode, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::sendBuffer(&param->mode, sizeof(drm_mode_modeinfo)),
				helix_ng::recvInline())
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_mode.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_OBJ_GETPROPERTIES: {
		auto param = reinterpret_cast<drm_mode_obj_get_properties *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_count_props(param->count_props);
		req.set_drm_obj_id(param->obj_id);
		req.set_drm_obj_type(param->obj_type);

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

		auto props = reinterpret_cast<uint32_t *>(param->props_ptr);
		auto prop_vals = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

		for(size_t i = 0; i < resp.drm_obj_property_ids_size(); i++) {
			if(i >= param->count_props) {
				break;
			}
			props[i] = resp.drm_obj_property_ids(i);
			prop_vals[i] = resp.drm_obj_property_values(i);
		}

		param->count_props = resp.drm_obj_property_ids_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_PAGE_FLIP: {
		auto param = reinterpret_cast<drm_mode_crtc_page_flip *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		__ensure(!(param->flags & ~DRM_MODE_PAGE_FLIP_EVENT));
		req.set_drm_crtc_id(param->crtc_id);
		req.set_drm_fb_id(param->fb_id);
		req.set_drm_cookie(param->user_data);

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_DIRTYFB: {
		auto param = reinterpret_cast<drm_mode_fb_dirty_cmd*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_fb_id(param->fb_id);
		req.set_drm_flags(param->flags);
		req.set_drm_color(param->color);
		for(size_t i = 0; i < param->num_clips; i++) {
			auto dest = reinterpret_cast<drm_clip_rect *>(param->clips_ptr);
			managarm::fs::Rect<MemoryAllocator> clip(getSysdepsAllocator());
			clip.set_x1(dest->x1);
			clip.set_y1(dest->y1);
			clip.set_x2(dest->x2);
			clip.set_y2(dest->y2);
			req.add_drm_clips(std::move(clip));
		}

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

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_MODE_CURSOR: {
		auto param = reinterpret_cast<drm_mode_cursor *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_flags(param->flags);
		req.set_drm_crtc_id(param->crtc_id);

		if (param->flags == DRM_MODE_CURSOR_MOVE) {
			req.set_drm_x(param->x);
			req.set_drm_y(param->y);
		} else if (param->flags == DRM_MODE_CURSOR_BO) {
			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_handle(param->handle);
		} else {
			mlibc::infoLogger() << "\e[35mmlibc: invalid flags in DRM_IOCTL_MODE_CURSOR\e[39m" << frg::endlog;
			return EINVAL;
		}

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

		if (resp.error() == managarm::fs::Errors::NO_BACKING_DEVICE) {
			return ENXIO;
		}else if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_MODE_DESTROY_DUMB: {
		auto param = reinterpret_cast<drm_mode_destroy_dumb *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_handle(param->handle);

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_CREATEPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_create_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_size(param->length);

		auto [offer, send_req, blob_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::sendBuffer(reinterpret_cast<void *>(param->data), param->length),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(blob_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->blob_id = resp.drm_blob_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_DESTROYPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_destroy_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_id(param->blob_id);

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ATOMIC: {
		auto param = reinterpret_cast<drm_mode_atomic *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_flags(param->flags);
		req.set_drm_cookie(param->user_data);

		size_t prop_count = 0;
		auto objs_ptr = reinterpret_cast<uint32_t *>(param->objs_ptr);
		auto count_props_ptr = reinterpret_cast<uint32_t *>(param->count_props_ptr);
		auto props_ptr = reinterpret_cast<uint32_t *>(param->props_ptr);
		auto prop_values_ptr = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

		for(size_t i = 0; i < param->count_objs; i++) {
			/* list of modeobjs and their property count */
			req.add_drm_obj_ids(objs_ptr[i]);
			req.add_drm_prop_counts(count_props_ptr[i]);
			prop_count += count_props_ptr[i];
		}

		for(size_t i = 0; i < prop_count; i++) {
			/* array of property IDs */
			req.add_drm_props(props_ptr[i]);
			/* array of property values */
			req.add_drm_prop_values(prop_values_ptr[i]);
		}

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

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_LIST_LESSEES: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_LIST_LESSEES"
				" is not implemented correctly\e[39m" << frg::endlog;
		return EINVAL;
	}
	case DRM_IOCTL_MODE_SETGAMMA: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_SETGAMMA"
				" is not implemented correctly\e[39m" << frg::endlog;
		return 0;
	}
	case DRM_IOCTL_MODE_CREATE_LEASE: {
		auto param = reinterpret_cast<drm_mode_create_lease *>(arg);

		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_CREATE_LEASE"
				" is a noop\e[39m" << frg::endlog;
		param->lessee_id = 1;
		param->fd = fd;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_GEM_CLOSE: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_GEM_CLOSE"
				" is a noop\e[39m" << frg::endlog;
		return 0;
	}
	case DRM_IOCTL_PRIME_HANDLE_TO_FD: {
		auto param = reinterpret_cast<drm_prime_handle *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_prime_handle(param->handle);
		req.set_drm_flags(param->flags);

		auto [offer, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::imbueCredentials(),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fd = resp.drm_prime_fd();
		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_PRIME_FD_TO_HANDLE: {
		auto param = reinterpret_cast<drm_prime_handle *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_flags(param->flags);

		auto [offer, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::imbueCredentials(getHandleForFd(param->fd)),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::FILE_NOT_FOUND) {
			return EBADF;
		} else {
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		}

		param->handle = resp.drm_prime_handle();
		*result = resp.result();
		return 0;
	}
	case TCGETS: {
		auto param = reinterpret_cast<struct termios *>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
		actions[3].buffer = param;
		actions[3].length = sizeof(struct termios);
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);
		auto recv_attrs = parseLength(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);
		HEL_CHECK(recv_attrs->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		__ensure(recv_attrs->length == sizeof(struct termios));
		*result = resp.result();
		return 0;
	}
	case TCSETS: {
		auto param = reinterpret_cast<struct termios *>(arg);
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionSendFromBuffer;
		actions[2].flags = kHelItemChain;
		actions[2].buffer = param;
		actions[2].length = sizeof(struct termios);
		actions[3].type = kHelActionRecvInline;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto send_attrs = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(send_attrs->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(send_attrs->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCSCTTY: {
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

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
		actions[3].type = kHelActionRecvInline;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto imbue_creds = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		if(imbue_creds->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else if(resp.error() == managarm::fs::Errors::INSUFFICIENT_PERMISSIONS) {
			return EPERM;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCGWINSZ: {
		auto param = reinterpret_cast<struct winsize *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

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
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET)
			return EINVAL;
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		param->ws_col = resp.pts_width();
		param->ws_row = resp.pts_height();
		param->ws_xpixel = resp.pts_pixel_width();
		param->ws_ypixel = resp.pts_pixel_height();
		return 0;
	}
	case TIOCSWINSZ: {
		auto param = reinterpret_cast<const struct winsize *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_pts_width(param->ws_col);
		req.set_pts_height(param->ws_row);
		req.set_pts_pixel_width(param->ws_xpixel);
		req.set_pts_pixel_height(param->ws_ypixel);

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
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case TIOCGPTN: {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

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
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*param = resp.pts_index();
		*result = resp.result();
		return 0;
	}
	case TIOCGPGRP: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		*static_cast<int *>(arg) = resp.pid();
		return 0;
	}
	case TIOCSPGRP: {
		auto param = reinterpret_cast<int *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_pgid((long int)param);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::INSUFFICIENT_PERMISSIONS) {
			return EPERM;
		} else if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCGSID: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		*static_cast<int *>(arg) = resp.pid();
		return 0;
	}
	case CDROM_GET_CAPABILITY: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
						helix_ng::sendBuffer(ser.data(), ser.size()),
						helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	} // end of switch()


	if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGVERSION)) {
		*reinterpret_cast<int *>(arg) = 0x010001; // should be EV_VERSION
		*result = 0;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGID)) {
		memset(arg, 0, sizeof(struct input_id));
		*result = 0;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGNAME(0))) {
		const char *s = "Managarm generic evdev";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPHYS(0))) {
		// Returns the sysfs path of the device.
		const char *s = "input0";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGUNIQ(0))) {
		// Returns a unique ID for the device.
		const char *s = "0";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPROP(0))) {
		// Returns a bitmask of properties of the device.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGKEY(0))) {
		// Returns the current key state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGLED(0))) {
		// Returns the current LED state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGSW(0))) {
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
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

			managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(0, 0));
			req.set_size(_IOC_SIZE(request));

			frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
			auto offer = parseHandle(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			if(recv_resp->error == kHelErrDismissed)
				return EINVAL;
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = recv_data->length;
			return 0;
		}else{
			// TODO: Check with the Linux ABI if we have to do this.
			memset(arg, 0, _IOC_SIZE(request));

			HelAction actions[4];
			globalQueue.trim();

			managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(1, 0));
			req.set_input_type(type);
			req.set_size(_IOC_SIZE(request));

			frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
			auto offer = parseHandle(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			if(recv_resp->error == kHelErrDismissed)
				return EINVAL;
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = recv_data->length;
			return 0;
		}
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOSCLOCKID)) {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_input_clock(*param);

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
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) >= _IOC_NR(EVIOCGABS(0))
			&& _IOC_NR(request) <= _IOC_NR(EVIOCGABS(ABS_MAX))) {
		auto param = reinterpret_cast<struct input_absinfo *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		auto type = _IOC_NR(request) - _IOC_NR(EVIOCGABS(0));
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(EVIOCGABS(0));
		req.set_input_type(type);

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
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->value = resp.input_value();
		param->minimum = resp.input_min();
		param->maximum = resp.input_max();
		param->fuzz = resp.input_fuzz();
		param->flat = resp.input_flat();
		param->resolution = resp.input_resolution();

		*result = resp.result();
		return 0;
	}else if(request == KDSETMODE) {
		auto param = reinterpret_cast<unsigned int *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_SETMODE(" << frg::hex_fmt(param) << ") is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == KDGETMODE) {
		auto param = reinterpret_cast<unsigned int *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_GETMODE is a no-op" << frg::endlog;
		*param = 0;

		*result = 0;
		return 0;
	}else if(request == KDSKBMODE) {
		auto param = reinterpret_cast<long>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_SKBMODE(" << frg::hex_fmt(param) << ") is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_SETMODE) {
		// auto param = reinterpret_cast<struct vt_mode *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: VT_SETMODE is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_GETSTATE) {
		auto param = reinterpret_cast<struct vt_stat *>(arg);

		param->v_active = 0;
		param->v_signal = 0;
		param->v_state = 0;

		mlibc::infoLogger() << "\e[35mmlibc: VT_GETSTATE is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_ACTIVATE || request == VT_WAITACTIVE) {
		mlibc::infoLogger() << "\e[35mmlibc: VT_ACTIVATE/VT_WAITACTIVE are no-ops" << frg::endlog;
		*result = 0;
		return 0;
	}else if(request == TIOCSPTLCK) {
		mlibc::infoLogger() << "\e[35mmlibc: TIOCSPTLCK is a no-op" << frg::endlog;
		*result = 0;
		return 0;
	}

	mlibc::infoLogger() << "mlibc: Unexpected ioctl with"
			<< " type: 0x" << frg::hex_fmt(_IOC_TYPE(request))
			<< ", number: 0x" << frg::hex_fmt(_IOC_NR(request))
			<< " (raw request: " << frg::hex_fmt(request) << ")" << frg::endlog;
	__ensure(!"Illegal ioctl request");
	__builtin_unreachable();
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if(flags & O_CREAT)
		proto_flags |= managarm::posix::OpenFlags::OF_CREATE;
	if(flags & O_EXCL)
		proto_flags |= managarm::posix::OpenFlags::OF_EXCLUSIVE;
	if(flags & O_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;
	if(flags & O_TRUNC)
		proto_flags |= managarm::posix::OpenFlags::OF_TRUNC;

	if(flags & O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if(flags & O_NOCTTY)
		proto_flags |= managarm::posix::OpenFlags::OF_NOCTTY;

	if(flags & O_RDONLY)
		proto_flags |= managarm::posix::OpenFlags::OF_RDONLY;
	else if(flags & O_WRONLY)
		proto_flags |= managarm::posix::OpenFlags::OF_WRONLY;
	else if(flags & O_RDWR)
		proto_flags |= managarm::posix::OpenFlags::OF_RDWR;
	else if(flags & O_PATH)
		proto_flags |= managarm::posix::OpenFlags::OF_PATH;

	managarm::posix::OpenAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(proto_flags);

	auto [offer, sendHead, sendTail, recvResp] = exchangeMsgsSync(
		getPosixLane(),
		helix_ng::offer(
			helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()),
			helix_ng::recvInline()
		)
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendHead.error());
	HEL_CHECK(sendTail.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		return EEXIST;
	}else if(resp.error() == managarm::posix::Errors::NOT_A_DIRECTORY) {
		return ENOTDIR;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_OPERATION_TARGET) {
		mlibc::infoLogger() << "\e[31mmlibc: openat unimplemented for this file " << path << "\e[39m" << frg::endlog;
		return EINVAL;
	}else if(resp.error() == managarm::posix::Errors::NO_BACKING_DEVICE) {
		return ENXIO;
	}else if(resp.error() == managarm::posix::Errors::IS_DIRECTORY) {
		return EISDIR;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*fd = resp.fd();
		return 0;
	}
}

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
	SignalGuard sguard;

	managarm::posix::MkfifoAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_mode(mode);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		return EEXIST;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
	SignalGuard sguard;

	managarm::posix::MknodAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_dirfd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_mode(mode);
	req.set_device(dev);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::ALREADY_EXISTS) {
		return EEXIST;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_read(int fd, void *data, size_t max_size, ssize_t *bytes_read) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::READ);
	req.set_fd(fd);
	req.set_size(max_size);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	auto [offer, send_req, imbue_creds, recv_resp, recv_data] =
		exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBuffer(ser.data(), ser.size()),
				helix_ng::imbueCredentials(),
				helix_ng::recvInline(),
				helix_ng::recvBuffer(data, max_size)
			)
		);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
/*	if(resp.error() == managarm::fs::Errors::NO_SUCH_FD) {
		return EBADF;
	}else*/
	if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		return EINVAL;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
	}else if(resp.error() == managarm::fs::Errors::END_OF_FILE) {
		*bytes_read = 0;
		return 0;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		HEL_CHECK(recv_data.error());
		*bytes_read = recv_data.actualLength();
		return 0;
	}
}

int sys_write(int fd, const void *data, size_t size, ssize_t *bytes_written) {
	SignalGuard sguard;
	HelAction actions[5];
	globalQueue.trim();

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::WRITE);
	req.set_fd(fd);
	req.set_size(size);

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
	actions[3].buffer = const_cast<void *>(data);
	actions[3].length = size;
	actions[4].type = kHelActionRecvInline;
	actions[4].flags = 0;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto send_data = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(send_data->error);
	HEL_CHECK(recv_resp->error);


	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	// TODO: implement NO_SUCH_FD
/*	if(resp.error() == managarm::fs::Errors::NO_SUCH_FD) {
		return EBADF;
	}else*/ if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return EINVAL; // FD does not support writes.
	}else if(resp.error() == managarm::fs::Errors::NO_SPACE_LEFT) {
		return ENOSPC;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*bytes_written = resp.size();
		return 0;
	}
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	SignalGuard sguard;
	HelAction actions[5];
	globalQueue.trim();

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_PREAD);
	req.set_fd(fd);
	req.set_size(n);
	req.set_offset(off);

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
	actions[3].type = kHelActionRecvInline;
	actions[3].flags = kHelItemChain;
	actions[4].type = kHelActionRecvToBuffer;
	actions[4].flags = 0;
	actions[4].buffer = buf;
	actions[4].length = n;
	HEL_CHECK(helSubmitAsync(handle, actions, 5,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto imbue_creds = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(imbue_creds->error);
	HEL_CHECK(recv_resp->error);

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
/*	if(resp.error() == managarm::fs::Errors::NO_SUCH_FD) {
		return EBADF;
	}else*/
	if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		return EINVAL;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
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

int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_PWRITE);
	req.set_fd(fd);
	req.set_size(n);
	req.set_offset(off);

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
	req.SerializeToString(&ser);

	auto [offer, send_head, send_tail, imbue_creds, recv_resp] = exchangeMsgsSync(
		handle,
		helix_ng::offer(
			helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()),
			helix_ng::imbueCredentials(),
			helix_ng::recvInline()
		)
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
		return EINVAL;
	}else if(resp.error() == managarm::fs::Errors::WOULD_BLOCK) {
		return EAGAIN;
	}else if(resp.error() == managarm::fs::Errors::NO_SPACE_LEFT) {
		return ENOSPC;
	}else if(resp.error() == managarm::fs::Errors::SEEK_ON_PIPE) {
		return ESPIPE;
	}else if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		HEL_CHECK(send_tail.error());
		*bytes_written = n;
		return 0;
	}
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if(!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_rel_offset(offset);

	if(whence == SEEK_SET) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_ABS);
	}else if(whence == SEEK_CUR) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_REL);
	}else if(whence == SEEK_END) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_EOF);
	}else{
		return EINVAL;
	}

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
	if(resp.error() == managarm::fs::Errors::SEEK_ON_PIPE) {
		return ESPIPE;
	}else{
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*new_offset = resp.offset();
		return 0;
	}
}


int sys_close(int fd) {
	SignalGuard sguard;

	managarm::posix::CloseRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	if(resp.error() == managarm::posix::Errors::NO_SUCH_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::SUCCESS) {
		return 0;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

int sys_dup(int fd, int flags, int *newfd) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	__ensure(!(flags & ~(O_CLOEXEC)));

	uint32_t proto_flags = 0;
	if(flags & O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::DUP);
	req.set_fd(fd);
	req.set_flags(proto_flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if (resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	} else if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	}

	*newfd = resp.fd();
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::DUP2);
	req.set_fd(fd);
	req.set_newfd(newfd);
	req.set_flags(flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);

	if (resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	} else if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	}

	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *result) {
	SignalGuard sguard;

	managarm::posix::FstatAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	if (fsfdt == fsfd_target::path) {
		req.set_fd(AT_FDCWD);
		req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	} else if (fsfdt == fsfd_target::fd) {
		flags |= AT_EMPTY_PATH;
		req.set_fd(fd);
	} else {
		__ensure(fsfdt == fsfd_target::fd_path);
		req.set_fd(fd);
		req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	}

	if (flags & ~(AT_SYMLINK_NOFOLLOW | AT_EMPTY_PATH)) {
		return EINVAL;
	}

	if (!(flags & AT_EMPTY_PATH) && (!path || !strlen(path))) {
		return ENOENT;
	}

	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::BAD_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::NOT_A_DIRECTORY) {
		return ENOTDIR;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		memset(result, 0, sizeof(struct stat));

		switch(resp.file_type()) {
		case managarm::posix::FileType::FT_REGULAR:
			result->st_mode = S_IFREG; break;
		case managarm::posix::FileType::FT_DIRECTORY:
			result->st_mode = S_IFDIR; break;
		case managarm::posix::FileType::FT_SYMLINK:
			result->st_mode = S_IFLNK; break;
		case managarm::posix::FileType::FT_CHAR_DEVICE:
			result->st_mode = S_IFCHR; break;
		case managarm::posix::FileType::FT_BLOCK_DEVICE:
			result->st_mode = S_IFBLK; break;
		case managarm::posix::FileType::FT_SOCKET:
			result->st_mode = S_IFSOCK; break;
		case managarm::posix::FileType::FT_FIFO:
			result->st_mode = S_IFIFO; break;
		default:
			__ensure(!resp.file_type());
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

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	SignalGuard sguard;
	HelAction actions[4];
	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::READLINK);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

	frg::string<MemoryAllocator> ser(getSysdepsAllocator());
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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 4,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);
	auto recv_data = parseLength(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);
	HEL_CHECK(recv_data->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		*length = recv_data->length;
		return 0;
	}
}

int sys_rmdir(const char *path) {
	SignalGuard sguard;

	managarm::posix::RmdirRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_ftruncate(int fd, size_t size) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_TRUNCATE);
	req.set_size(size);

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
	if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
		return EINVAL;
	} else {
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		return 0;
	}
}

int sys_fallocate(int fd, off_t offset, size_t size) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_FALLOCATE);
	req.set_rel_offset(offset);
	req.set_size(size);

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
}

int sys_unlinkat(int fd, const char *path, int flags) {
	SignalGuard sguard;

	managarm::posix::UnlinkAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::RESOURCE_IN_USE) {
		return EBUSY;
	}else if(resp.error() == managarm::posix::Errors::IS_DIRECTORY) {
		return EISDIR;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_access(const char *path, int mode) {
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int sys_faccessat(int dirfd, const char *pathname, int, int flags) {
	SignalGuard sguard;
	HelAction actions[3];

	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::ACCESSAT);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	req.set_fd(dirfd);
	req.set_flags(flags);

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
	HEL_CHECK(helSubmitAsync(getPosixLane(), actions, 3,
			globalQueue.getQueue(), 0, 0));

	auto element = globalQueue.dequeueSingle();
	auto offer = parseHandle(element);
	auto send_req = parseSimple(element);
	auto recv_resp = parseInline(element);

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::NO_SUCH_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_flock(int fd, int opts) {
	SignalGuard sguard;
	HelAction actions[3];
	globalQueue.trim();

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::FLOCK);
	req.set_fd(fd);
	req.set_flock_flags(opts);
	auto handle = getHandleForFd(fd);
	if(!handle) {
		return EBADF;
	}

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	if(resp.error() == managarm::posix::Errors::WOULD_BLOCK) {
		return EWOULDBLOCK;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_isatty(int fd) {
	SignalGuard sguard;

	managarm::posix::IsTtyRequest<MemoryAllocator> req(getSysdepsAllocator());
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
	if(resp.error() ==  managarm::posix::Errors::NO_SUCH_FD) {
		return EBADF;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		if(resp.mode())
			return 0;
		return ENOTTY;
	}
}

int sys_chmod(const char *pathname, mode_t mode) {
	return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}

int sys_fchmod(int fd, mode_t mode) {
	return sys_fchmodat(fd, "", mode, AT_EMPTY_PATH);
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	SignalGuard sguard;

	managarm::posix::FchmodAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	req.set_mode(mode);
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::NO_SUCH_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else if(resp.error() == managarm::posix::Errors::NOT_SUPPORTED) {
		return ENOTSUP;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	SignalGuard sguard;

	managarm::posix::UtimensAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	if(pathname != nullptr)
		req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	if(times) {
		req.set_atimeSec(times[0].tv_sec);
		req.set_atimeNsec(times[0].tv_nsec);
		req.set_mtimeSec(times[1].tv_sec);
		req.set_mtimeNsec(times[1].tv_nsec);
	} else {
		req.set_atimeSec(UTIME_NOW);
		req.set_atimeNsec(UTIME_NOW);
		req.set_mtimeSec(UTIME_NOW);
		req.set_mtimeNsec(UTIME_NOW);
	}
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		return ENOENT;
	}else if(resp.error() == managarm::posix::Errors::NO_SUCH_FD) {
		return EBADF;
	}else if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}else if(resp.error() == managarm::posix::Errors::NOT_SUPPORTED) {
		return ENOTSUP;
	}else{
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
}

int sys_getentropy(void *buffer, size_t length) {
	SignalGuard sguard;
	auto p = reinterpret_cast<char *>(buffer);
	size_t n = 0;

	while(n < length) {
		size_t chunk;
		HEL_CHECK(helGetRandomBytes(p + n, length - n, &chunk));
		n+= chunk;
	}

	return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	SignalGuard sguard;
	mlibc::infoLogger() << "mlibc: gethostname always returns managarm" << frg::endlog;
	char name[10] = "managarm\0";
	if(bufsize < 10)
		return ENAMETOOLONG;
	strncpy(buffer, name, 10);
	return 0;
}

int sys_fsync(int) {
	mlibc::infoLogger() << "mlibc: fsync is a stub" << frg::endlog;
	return 0;
}

int sys_memfd_create(const char *name, int flags, int *fd) {
	SignalGuard sguard;

	managarm::posix::MemFdCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_name(frg::string<MemoryAllocator>(getSysdepsAllocator(), name));
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if(resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	}

	*fd = resp.fd();

	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}

int sys_uname(struct utsname *buf) {
	__ensure(buf);
	mlibc::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frg::endlog;
	strcpy(buf->sysname, "Managarm");
	strcpy(buf->nodename, "?");
	strcpy(buf->release, "?");
	strcpy(buf->version, "?");
#if defined(__x86_64__)
	strcpy(buf->machine, "x86_64");
#elif defined (__aarch64__)
	strcpy(buf->machine, "aarch64");
#else
#	error Unknown architecture
#endif

	return 0;
}

int sys_madvise(void *addr, size_t length, int advice) {
	mlibc::infoLogger() << "mlibc: sys_madvise is a stub!" << frg::endlog;
	return 0;
}

} //namespace mlibc

