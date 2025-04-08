#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif

#include <asm/ioctls.h>
#include <dirent.h>
#include <errno.h>
#include <frg/small_vector.hpp>
#include <pthread.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/reboot.h>
#include <sys/signalfd.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include <bits/ensure.h>
#include <bits/errors.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-pipe.hpp>

#include <fs.frigg_bragi.hpp>
#include <posix.frigg_bragi.hpp>

HelHandle __mlibc_getPassthrough(int fd) {
	auto handle = getHandleForFd(fd);
	__ensure(handle);
	return handle;
}

namespace mlibc {

int sys_chdir(const char *path) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::CHDIR);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_fchdir(int fd) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::FCHDIR);
	req.set_fd(fd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	return 0;
}

int sys_chroot(const char *path) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::CHROOT);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	return 0;
}

int sys_mkdir(const char *path, mode_t mode) { return sys_mkdirat(AT_FDCWD, path, mode); }

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	(void)mode;
	SignalGuard sguard;

	managarm::posix::MkdirAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

} // namespace mlibc

namespace mlibc {

int do_dup2(int fd, int flags, int newfd, bool fcntl_mode, int *outfd) {
	SignalGuard sguard;

	managarm::posix::Dup2Request<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_newfd(newfd);
	req.set_flags(flags);
	req.set_fcntl_mode(fcntl_mode);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::Dup2Response resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	if (outfd)
		*outfd = resp.fd();

	return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	SignalGuard sguard;
	if (request == F_DUPFD) {
		int newfd;
		int wantedFd = va_arg(args, int);
		if (int e = do_dup2(fd, 0, wantedFd, true, &newfd); e)
			return e;
		*result = newfd;
		return 0;
	} else if (request == F_DUPFD_CLOEXEC) {
		int newfd;
		int wantedFd = va_arg(args, int);
		if (int e = do_dup2(fd, O_CLOEXEC, wantedFd, true, &newfd); e)
			return e;
		*result = newfd;
		return 0;
	} else if (request == F_GETFD) {
		managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_request_type(managarm::posix::CntReqType::FD_GET_FLAGS);
		req.set_fd(fd);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    getPosixLane(),
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if (resp.error() != managarm::posix::Errors::SUCCESS)
			return resp.error() | toErrno;
		*result = resp.flags();
		return 0;
	} else if (request == F_SETFD) {
		managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_request_type(managarm::posix::CntReqType::FD_SET_FLAGS);
		req.set_fd(fd);
		req.set_flags(va_arg(args, int));

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    getPosixLane(),
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if (resp.error() != managarm::posix::Errors::SUCCESS)
			return resp.error() | toErrno;
		*result = static_cast<int>(resp.error());
		return 0;
	} else if (request == F_GETFL) {
		SignalGuard sguard;

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_FILE_FLAGS);
		req.set_fd(fd);

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
		if (resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_GETFL) unimplemented for this file\e[39m"
			                    << frg::endlog;
			return EINVAL;
		} else if (resp.error() != managarm::fs::Errors::SUCCESS) {
			return resp.error() | toErrno;
		}

		*result = resp.flags();
		return 0;
	} else if (request == F_SETFL) {
		SignalGuard sguard;

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_SET_FILE_FLAGS);
		req.set_fd(fd);
		req.set_flags(va_arg(args, int));

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
		if (resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger() << "\e[31mmlibc: fcntl(F_SETFL) unimplemented for this file\e[39m"
			                    << frg::endlog;
			return EINVAL;
		} else if (resp.error() != managarm::fs::Errors::SUCCESS) {
			return resp.error() | toErrno;
		}

		*result = 0;
		return 0;
	} else if (request == F_SETLK) {
		mlibc::infoLogger() << "\e[31mmlibc: F_SETLK\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_SETLKW) {
		mlibc::infoLogger() << "\e[31mmlibc: F_SETLKW\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_GETLK) {
		struct flock *lock = va_arg(args, struct flock *);
		lock->l_type = F_UNLCK;
		mlibc::infoLogger() << "\e[31mmlibc: F_GETLK is stubbed!\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_OFD_SETLK) {
		mlibc::infoLogger() << "\e[31mmlibc: F_OFD_SETLK\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_OFD_SETLKW) {
		mlibc::infoLogger() << "\e[31mmlibc: F_OFD_SETLKW\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_OFD_GETLK) {
		struct flock *lock = va_arg(args, struct flock *);
		lock->l_type = F_UNLCK;
		mlibc::infoLogger() << "\e[31mmlibc: F_OFD_GETLK is stubbed!\e[39m" << frg::endlog;
		return 0;
	} else if (request == F_ADD_SEALS) {
		auto seals = va_arg(args, int);
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_ADD_SEALS);
		req.set_fd(fd);
		req.set_seals(seals);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::RecvInline()
		    )
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if (resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger(
			) << "\e[31mmlibc: fcntl(F_ADD_SEALS) unimplemented for this file\e[39m"
			  << frg::endlog;
			return EINVAL;
		} else if (resp.error() != managarm::fs::Errors::SUCCESS) {
			return resp.error() | toErrno;
		}

		*result = resp.seals();
		return 0;
	} else if (request == F_GET_SEALS) {
		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_GET_SEALS);
		req.set_fd(fd);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::RecvInline()
		    )
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if (resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET) {
			mlibc::infoLogger(
			) << "\e[31mmlibc: fcntl(F_GET_SEALS) unimplemented for this file\e[39m"
			  << frg::endlog;
			return EINVAL;
		} else if (resp.error() != managarm::fs::Errors::SUCCESS) {
			return resp.error() | toErrno;
		}

		*result = resp.seals();
		return 0;
	} else {
		mlibc::infoLogger() << "\e[31mmlibc: Unexpected fcntl() request: " << request << "\e[39m"
		                    << frg::endlog;
		return EINVAL;
	}
}

int sys_open_dir(const char *path, int *handle) { return sys_open(path, 0, 0, handle); }

int sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read) {
	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_READ_ENTRIES);

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
	if (resp.error() == managarm::fs::Errors::END_OF_FILE) {
		*bytes_read = 0;
		return 0;
	} else if (resp.error() != managarm::fs::Errors::SUCCESS) {
		return resp.error() | toErrno;
	}

	__ensure(max_size > sizeof(struct dirent));
	auto ent = new (buffer) struct dirent;
	memset(ent, 0, sizeof(struct dirent));
	memcpy(ent->d_name, resp.path().data(), resp.path().size());
	ent->d_reclen = sizeof(struct dirent);
	*bytes_read = sizeof(struct dirent);
	return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::TTY_NAME);
	req.set_fd(fd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	__ensure(size >= resp.path().size() + 1);
	memcpy(buf, resp.path().data(), size);
	buf[resp.path().size()] = '\0';
	return 0;
}

int sys_fdatasync(int) {
	mlibc::infoLogger() << "\e[35mmlibc: fdatasync() is a no-op\e[39m" << frg::endlog;
	return 0;
}

int sys_getcwd(char *buffer, size_t size) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::GETCWD);
	req.set_size(size);

	auto [offer, send_req, recv_resp, recv_path] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline(),
	        helix_ng::recvBuffer(buffer, size)
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());
	HEL_CHECK(recv_path.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	if (static_cast<size_t>(resp.size()) >= size)
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
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_REMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_new_size(new_size);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*window = reinterpret_cast<void *>(resp.offset());
	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_PROTECT);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);
	req.set_mode(prot);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::VM_UNMAP);
	req.set_address(reinterpret_cast<uintptr_t>(pointer));
	req.set_size(size);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_setsid(pid_t *sid) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SETSID);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS) {
		*sid = -1;
		return resp.error() | toErrno;
	}

	*sid = resp.sid();
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	int result;
	if (int e = sys_ioctl(fd, TCGETS, attr, &result); e)
		return e;
	return 0;
}

int sys_tcsetattr(int fd, int when, const struct termios *attr) {
	if (when < TCSANOW || when > TCSAFLUSH)
		return EINVAL;

	if (int e = sys_ioctl(fd, TCSETS, const_cast<struct termios *>(attr), nullptr); e)
		return e;
	return 0;
}

int sys_tcdrain(int) {
	mlibc::infoLogger() << "\e[35mmlibc: tcdrain() is a stub\e[39m" << frg::endlog;
	return 0;
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
	constexpr int type_mask = int(0xF);
	constexpr int flags_mask = ~int(0xF);

	SignalGuard sguard;

	managarm::posix::SocketRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_domain(domain);
	req.set_socktype(type_and_flags & type_mask);
	req.set_protocol(proto);
	req.set_flags(type_and_flags & flags_mask);

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
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*fd = resp.fd();
	return 0;
}

int sys_pipe(int *fds, int flags) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::PIPE_CREATE);
	req.set_flags(flags);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	__ensure(resp.fds_size() == 2);
	fds[0] = resp.fds(0);
	fds[1] = resp.fds(1);
	return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	constexpr int type_mask = int(0xF);
	constexpr int flags_mask = ~int(0xF);
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
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	__ensure(resp.fds_size() == 2);
	fds[0] = resp.fds(0);
	fds[1] = resp.fds(1);
	return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length) {
	frg::small_vector<HelSgItem, 8, MemoryAllocator> sglist{getSysdepsAllocator()};
	auto handle = getHandleForFd(sockfd);
	if (!handle)
		return EBADF;

	size_t overall_size = 0;
	for (size_t i = 0; i < hdr->msg_iovlen; i++) {
		HelSgItem item{
		    .buffer = hdr->msg_iov[i].iov_base,
		    .length = hdr->msg_iov[i].iov_len,
		};
		sglist.push_back(item);
		overall_size += hdr->msg_iov[i].iov_len;
	}

	SignalGuard sguard;

	managarm::fs::SendMsgRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(flags);
	req.set_size(overall_size);

	req.set_has_cmsg_creds(false);
	req.set_has_cmsg_rights(false);
	for (auto cmsg = CMSG_FIRSTHDR(hdr); cmsg; cmsg = CMSG_NXTHDR(hdr, cmsg)) {
		__ensure(cmsg->cmsg_level == SOL_SOCKET);
		__ensure(cmsg->cmsg_len >= sizeof(struct cmsghdr));
		if (cmsg->cmsg_type == SCM_CREDENTIALS) {
			req.set_has_cmsg_creds(true);
			size_t size = cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr));
			__ensure(size == sizeof(struct ucred));
			struct ucred creds;
			memcpy(&creds, CMSG_DATA(cmsg), sizeof(struct ucred));
			req.set_creds_pid(creds.pid);
			req.set_creds_uid(creds.uid);
			req.set_creds_gid(creds.gid);
		} else if (cmsg->cmsg_type == SCM_RIGHTS) {
			req.set_has_cmsg_rights(true);
			size_t size = cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr));
			__ensure(!(size % sizeof(int)));
			for (size_t off = 0; off < size; off += sizeof(int)) {
				int fd;
				memcpy(&fd, CMSG_DATA(cmsg) + off, sizeof(int));
				req.add_fds(fd);
			}
		} else {
			mlibc::infoLogger(
			) << "mlibc: sys_msg_send only supports SCM_RIGHTS or SCM_CREDENTIALS, got: "
			  << cmsg->cmsg_type << "!" << frg::endlog;
			return EINVAL;
		}
	}

	auto [offer, send_head, send_tail, send_data, imbue_creds, send_addr, recv_resp] =
	    exchangeMsgsSync(
	        handle,
	        helix_ng::offer(
	            helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()),
	            helix_ng::sendBufferSg(sglist.data(), hdr->msg_iovlen),
	            helix_ng::imbueCredentials(),
	            helix_ng::sendBuffer(hdr->msg_name, hdr->msg_namelen),
	            helix_ng::recvInline()
	        )
	    );

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(send_tail.error());
	HEL_CHECK(send_data.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(send_addr.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SendMsgReply<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	*length = resp.size();
	return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
	if (!hdr->msg_iovlen) {
		return EMSGSIZE;
	}

	auto handle = getHandleForFd(sockfd);
	if (!handle)
		return EBADF;

	SignalGuard sguard;

	managarm::fs::RecvMsgRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(flags);
	req.set_size(hdr->msg_iov[0].iov_len);
	req.set_addr_size(hdr->msg_namelen);
	req.set_ctrl_size(hdr->msg_controllen);

	auto [offer, send_req, imbue_creds, recv_resp, recv_addr, recv_data, recv_ctrl] =
	    exchangeMsgsSync(
	        handle,
	        helix_ng::offer(
	            helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	            helix_ng::imbueCredentials(),
	            helix_ng::recvInline(),
	            helix_ng::recvBuffer(hdr->msg_name, hdr->msg_namelen),
	            helix_ng::recvBuffer(hdr->msg_iov[0].iov_base, hdr->msg_iov[0].iov_len),
	            helix_ng::recvBuffer(hdr->msg_control, hdr->msg_controllen)
	        )
	    );

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::RecvMsgReply<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::fs::Errors::SUCCESS) {
		return resp.error() | toErrno;
	} else {
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		HEL_CHECK(recv_addr.error());
		HEL_CHECK(recv_data.error());
		HEL_CHECK(recv_ctrl.error());

		hdr->msg_namelen = resp.addr_size();
		hdr->msg_controllen = recv_ctrl.actualLength();
		hdr->msg_flags = resp.flags();
		*length = resp.ret_val();
		return 0;
	}
}

int sys_pselect(
    int,
    fd_set *read_set,
    fd_set *write_set,
    fd_set *except_set,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	// TODO: Do not keep errors from epoll (?).
	int fd = epoll_create1(0);
	if (fd == -1)
		return -1;

	for (int k = 0; k < FD_SETSIZE; k++) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));

		if (read_set && FD_ISSET(k, read_set))
			ev.events |= EPOLLIN; // TODO: Additional events.
		if (write_set && FD_ISSET(k, write_set))
			ev.events |= EPOLLOUT; // TODO: Additional events.
		if (except_set && FD_ISSET(k, except_set))
			ev.events |= EPOLLPRI;

		if (!ev.events)
			continue;
		ev.data.u32 = k;

		if (epoll_ctl(fd, EPOLL_CTL_ADD, k, &ev))
			return -1;
	}

	struct epoll_event evnts[16];
	int n = epoll_pwait(
	    fd, evnts, 16, timeout ? (timeout->tv_sec * 1000 + timeout->tv_nsec / 100) : -1, sigmask
	);
	if (n == -1)
		return -1;

	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);
	int m = 0;

	for (int i = 0; i < n; i++) {
		int k = evnts[i].data.u32;

		if (read_set && FD_ISSET(k, read_set)
		    && evnts[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_read_set);
			m++;
		}

		if (write_set && FD_ISSET(k, write_set)
		    && evnts[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_write_set);
			m++;
		}

		if (except_set && FD_ISSET(k, except_set) && evnts[i].events & EPOLLPRI) {
			FD_SET(k, &res_except_set);
			m++;
		}
	}

	if (close(fd))
		__ensure("close() failed on epoll file");

	if (read_set)
		memcpy(read_set, &res_read_set, sizeof(fd_set));
	if (write_set)
		memcpy(write_set, &res_write_set, sizeof(fd_set));
	if (except_set)
		memcpy(except_set, &res_except_set, sizeof(fd_set));

	*num_events = m;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	__ensure(timeout >= 0 || timeout == -1); // TODO: Report errors correctly.

	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_CALL);
	req.set_timeout(timeout > 0 ? int64_t{timeout} * 1000000 : timeout);

	for (nfds_t i = 0; i < count; i++) {
		req.add_fds(fds[i].fd);
		req.add_events(fds[i].events);
	}

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() == managarm::posix::Errors::ILLEGAL_ARGUMENTS) {
		return EINVAL;
	} else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		__ensure(resp.events_size() == count);

		int m = 0;
		for (nfds_t i = 0; i < count; i++) {
			if (resp.events(i))
				m++;
			fds[i].revents = resp.events(i);
		}

		*num_events = m;
		return 0;
	}
}

int sys_epoll_create(int flags, int *fd) {
	// Some applications assume EPOLL_CLOEXEC and O_CLOEXEC to be the same.
	// They are on linux, but not yet on managarm.
	__ensure(!(flags & ~(EPOLL_CLOEXEC | O_CLOEXEC)));

	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if (flags & EPOLL_CLOEXEC || flags & O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_CREATE);
	req.set_flags(proto_flags);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	*fd = resp.fd();
	return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	if (mode == EPOLL_CTL_ADD) {
		__ensure(ev);
		req.set_request_type(managarm::posix::CntReqType::EPOLL_ADD);
		req.set_flags(ev->events);
		req.set_cookie(ev->data.u64);
	} else if (mode == EPOLL_CTL_MOD) {
		__ensure(ev);
		req.set_request_type(managarm::posix::CntReqType::EPOLL_MODIFY);
		req.set_flags(ev->events);
		req.set_cookie(ev->data.u64);
	} else if (mode == EPOLL_CTL_DEL) {
		req.set_request_type(managarm::posix::CntReqType::EPOLL_DELETE);
	} else {
		mlibc::panicLogger() << "\e[31mmlibc: Illegal epoll_ctl() mode\e[39m" << frg::endlog;
	}
	req.set_fd(epfd);
	req.set_newfd(fd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_epoll_pwait(
    int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised
) {
	__ensure(timeout >= 0 || timeout == -1); // TODO: Report errors correctly.

	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::EPOLL_WAIT);
	req.set_fd(epfd);
	req.set_size(n);
	req.set_timeout(timeout > 0 ? int64_t{timeout} * 1000000 : timeout);
	if (sigmask != NULL) {
		req.set_sigmask(*reinterpret_cast<const int64_t *>(sigmask));
		req.set_sigmask_needed(true);
	} else {
		req.set_sigmask_needed(false);
	}

	auto [offer, send_req, recv_resp, recv_data] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::recvInline(),
	        helix_ng::recvBuffer(ev, n * sizeof(struct epoll_event))
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());
	HEL_CHECK(recv_data.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	__ensure(!(recv_data.actualLength() % sizeof(struct epoll_event)));
	*raised = recv_data.actualLength() / sizeof(struct epoll_event);
	return 0;
}

int sys_timerfd_create(int clockid, int flags, int *fd) {
	SignalGuard sguard;

	managarm::posix::TimerFdCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_clock(clockid);
	req.set_flags(flags);

	auto [offer, sendReq, recvResp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::TimerFdCreateResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	*fd = resp.fd();
	return 0;
}

int sys_timerfd_settime(
    int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue
) {
	SignalGuard sguard;

	managarm::posix::TimerFdSetRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_flags(flags);
	req.set_value_sec(value->it_value.tv_sec);
	req.set_value_nsec(value->it_value.tv_nsec);
	req.set_interval_sec(value->it_interval.tv_sec);
	req.set_interval_nsec(value->it_interval.tv_nsec);

	auto [offer, sendReq, recvResp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendReq.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::TimerFdSetResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (oldvalue) {
		oldvalue->it_value.tv_sec = resp.value_sec();
		oldvalue->it_value.tv_nsec = resp.value_nsec();
		oldvalue->it_interval.tv_sec = resp.interval_sec();
		oldvalue->it_interval.tv_nsec = resp.interval_nsec();
	}

	return 0;
}

int sys_timerfd_gettime(int fd, struct itimerspec *its) {
	SignalGuard sguard;

	managarm::posix::TimerFdGetRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	managarm::posix::TimerFdGetResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (its) {
		its->it_value.tv_sec = resp.value_sec();
		its->it_value.tv_nsec = resp.value_nsec();
		its->it_interval.tv_sec = resp.interval_sec();
		its->it_interval.tv_nsec = resp.interval_nsec();
	} else {
		return EFAULT;
	}

	return 0;
}

int sys_signalfd_create(const sigset_t *masks, int flags, int *fd) {
	__ensure(!(flags & ~(SFD_CLOEXEC | SFD_NONBLOCK)));

	uint32_t proto_flags = 0;
	if (flags & SFD_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if (flags & SFD_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;

	SignalGuard sguard;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::SIGNALFD_CREATE);
	req.set_flags(proto_flags);
	req.set_sigset(*reinterpret_cast<const uint64_t *>(masks));
	req.set_fd(*fd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	*fd = resp.fd();
	return 0;
}

int sys_pidfd_open(pid_t pid, unsigned int flags, int *outfd) {
	SignalGuard sguard;

	managarm::posix::PidfdOpenRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_pid(pid);
	req.set_flags(flags);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::PidfdOpenResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*outfd = resp.fd();
	return 0;
}

int sys_pidfd_getpid(int pidfd, pid_t *outpid) {
	SignalGuard sguard;

	managarm::posix::PidfdGetPidRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_pidfd(pidfd);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::PidfdGetPidResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*outpid = resp.pid();
	return 0;
}

int sys_pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags) {
	SignalGuard sguard;

	if (info) {
		mlibc::infoLogger() << "mlibc: pidfd_send_signal does not support passing siginfo_t info"
		                    << frg::endlog;
		return EINVAL;
	}

	managarm::posix::PidfdSendSignalRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_pidfd(pidfd);
	req.set_signal(sig);
	req.set_flags(flags);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::PidfdSendSignalResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_reboot(int command) {
	if (command != RB_POWER_OFF && command != RB_AUTOBOOT) {
		mlibc::infoLogger(
		) << "mlibc: Anything other than power off or reboot is not supported yet!"
		  << frg::endlog;
		return EINVAL;
	}

	SignalGuard sguard;

	managarm::posix::RebootRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_cmd(command);

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
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	return 0;
}

int sys_inotify_create(int flags, int *fd) {
	__ensure(!(flags & ~(IN_CLOEXEC | IN_NONBLOCK)));

	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if (flags & IN_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if (flags & IN_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;

	managarm::posix::InotifyCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(proto_flags);

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
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	*fd = resp.fd();
	return 0;
}

int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd) {
	SignalGuard sguard;

	managarm::posix::InotifyAddRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(ifd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(mask);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*wd = resp.wd();
	return 0;
}

int sys_inotify_rm_watch(int ifd, int wd) {
	SignalGuard sguard;

	managarm::posix::InotifyRmRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_ifd(ifd);
	req.set_wd(wd);

	auto [offer, send_head, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::InotifyRmReply<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_eventfd_create(unsigned int initval, int flags, int *fd) {
	SignalGuard sguard;

	uint32_t proto_flags = 0;
	if (flags & EFD_NONBLOCK)
		proto_flags |= managarm::posix::EventFdFlags::NONBLOCK;
	if (flags & EFD_CLOEXEC)
		proto_flags |= managarm::posix::EventFdFlags::CLOEXEC;
	if (flags & EFD_SEMAPHORE)
		proto_flags |= managarm::posix::EventFdFlags::SEMAPHORE;

	managarm::posix::EventfdCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_flags(proto_flags);
	req.set_initval(initval);

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
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	*fd = resp.fd();
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	SignalGuard sguard;

	// We do not support O_TMPFILE.
	if (flags & O_TMPFILE)
		return EOPNOTSUPP;

	uint32_t proto_flags = 0;
	if (flags & O_APPEND)
		proto_flags |= managarm::posix::OpenFlags::OF_APPEND;
	if (flags & O_CREAT)
		proto_flags |= managarm::posix::OpenFlags::OF_CREATE;
	if (flags & O_EXCL)
		proto_flags |= managarm::posix::OpenFlags::OF_EXCLUSIVE;
	if (flags & O_NONBLOCK)
		proto_flags |= managarm::posix::OpenFlags::OF_NONBLOCK;
	if (flags & O_TRUNC)
		proto_flags |= managarm::posix::OpenFlags::OF_TRUNC;

	if (flags & O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;
	if (flags & O_NOCTTY)
		proto_flags |= managarm::posix::OpenFlags::OF_NOCTTY;
	if (flags & O_NOFOLLOW)
		proto_flags |= managarm::posix::OpenFlags::OF_NOFOLLOW;
	if (flags & O_DIRECTORY)
		proto_flags |= managarm::posix::OpenFlags::OF_DIRECTORY;

	if (flags & O_PATH)
		proto_flags |= managarm::posix::OpenFlags::OF_PATH;
	else if ((flags & O_ACCMODE) == O_RDONLY)
		proto_flags |= managarm::posix::OpenFlags::OF_RDONLY;
	else if ((flags & O_ACCMODE) == O_WRONLY)
		proto_flags |= managarm::posix::OpenFlags::OF_WRONLY;
	else if ((flags & O_ACCMODE) == O_RDWR)
		proto_flags |= managarm::posix::OpenFlags::OF_RDWR;

	managarm::posix::OpenAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(proto_flags);
	req.set_mode(mode);

	auto [offer, sendHead, sendTail, recvResp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(sendHead.error());
	HEL_CHECK(sendTail.error());
	HEL_CHECK(recvResp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recvResp.data(), recvResp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*fd = resp.fd();
	return 0;
}

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
	SignalGuard sguard;

	managarm::posix::MkfifoAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_mode(mode);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
	SignalGuard sguard;

	managarm::posix::MknodAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_dirfd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_mode(mode);
	req.set_device(dev);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
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

	auto [offer, send_req, imbue_creds, recv_resp, recv_data] = exchangeMsgsSync(
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
	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	HEL_CHECK(recv_data.error());
	*bytes_read = recv_data.actualLength();
	return 0;
}

int sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
	for (int i = 0; i < iovc; i++) {
		ssize_t intermed = 0;

		if (int e = sys_read(fd, iovs[i].iov_base, iovs[i].iov_len, &intermed); e)
			return e;
		else if (intermed == 0)
			break;

		*bytes_read += intermed;
	}

	return 0;
}

int sys_write(int fd, const void *data, size_t size, ssize_t *bytes_written) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::WRITE);
	req.set_fd(fd);
	req.set_size(size);

	auto [offer, send_req, imbue_creds, send_data, recv_resp] = exchangeMsgsSync(
	    handle,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::imbueCredentials(),
	        helix_ng::sendBuffer(data, size),
	        helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(send_data.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (bytes_written)
		*bytes_written = resp.size();

	return 0;
}

int sys_writev(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
	frg::small_vector<HelSgItem, 8, MemoryAllocator> sglist{getSysdepsAllocator()};

	size_t overall_size = 0;
	for (int i = 0; i < iovc; i++) {
		HelSgItem item{
		    .buffer = iovs[i].iov_base,
		    .length = iovs[i].iov_len,
		};
		sglist.push_back(item);
		overall_size += iovs[i].iov_len;
	}

	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::WRITE);
	req.set_fd(fd);
	req.set_size(overall_size);

	auto [offer, send_req, imbue_creds, send_data, recv_resp] = exchangeMsgsSync(
	    handle,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::imbueCredentials(),
	        helix_ng::sendBufferSg(sglist.data(), iovc),
	        helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(send_data.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (bytes_written)
		*bytes_written = resp.size();

	return 0;
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_PREAD);
	req.set_fd(fd);
	req.set_size(n);
	req.set_offset(off);

	auto [offer, send_req, imbue_creds, recv_resp, recv_data] = exchangeMsgsSync(
	    handle,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::imbueCredentials(),
	        helix_ng::recvInline(),
	        helix_ng::recvBuffer(buf, n)
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() == managarm::fs::Errors::END_OF_FILE) {
		*bytes_read = 0;
		return 0;
	} else if (resp.error() == managarm::fs::Errors::SUCCESS) {
		HEL_CHECK(recv_data.error());
		*bytes_read = recv_data.actualLength();
		return 0;
	}

	return resp.error() | toErrno;
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

	auto [offer, send_head, imbue_creds, to_write, recv_resp] = exchangeMsgsSync(
	    handle,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
	        helix_ng::imbueCredentials(),
	        helix_ng::sendBuffer(buf, n),
	        helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(imbue_creds.error());
	HEL_CHECK(to_write.error());
	HEL_CHECK(recv_resp.error());

	managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	*bytes_written = n;
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_rel_offset(offset);

	if (whence == SEEK_SET) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_ABS);
	} else if (whence == SEEK_CUR) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_REL);
	} else if (whence == SEEK_END) {
		req.set_req_type(managarm::fs::CntReqType::SEEK_EOF);
	} else {
		return EINVAL;
	}

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
	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	*new_offset = resp.offset();
	return 0;
}

int sys_close(int fd) {
	SignalGuard sguard;

	managarm::posix::CloseRequest<MemoryAllocator> req(getSysdepsAllocator());
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

	return resp.error() | toErrno;
}

int sys_dup(int fd, int flags, int *newfd) {
	SignalGuard sguard;

	__ensure(!(flags & ~(O_CLOEXEC)));

	uint32_t proto_flags = 0;
	if (flags & O_CLOEXEC)
		proto_flags |= managarm::posix::OpenFlags::OF_CLOEXEC;

	managarm::posix::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_request_type(managarm::posix::CntReqType::DUP);
	req.set_fd(fd);
	req.set_flags(proto_flags);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*newfd = resp.fd();
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) { return do_dup2(fd, flags, newfd, false, nullptr); }

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

	if (!(flags & AT_EMPTY_PATH) && (!path || !strlen(path))) {
		return ENOENT;
	}

	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	memset(result, 0, sizeof(struct stat));

	switch (resp.file_type()) {
		case managarm::posix::FileType::FT_REGULAR:
			result->st_mode = S_IFREG;
			break;
		case managarm::posix::FileType::FT_DIRECTORY:
			result->st_mode = S_IFDIR;
			break;
		case managarm::posix::FileType::FT_SYMLINK:
			result->st_mode = S_IFLNK;
			break;
		case managarm::posix::FileType::FT_CHAR_DEVICE:
			result->st_mode = S_IFCHR;
			break;
		case managarm::posix::FileType::FT_BLOCK_DEVICE:
			result->st_mode = S_IFBLK;
			break;
		case managarm::posix::FileType::FT_SOCKET:
			result->st_mode = S_IFSOCK;
			break;
		case managarm::posix::FileType::FT_FIFO:
			result->st_mode = S_IFIFO;
			break;
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

int
sys_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
	SignalGuard sguard;

	managarm::posix::FstatAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	req.set_fd(dirfd);

	if (flags
	    & ~(AT_SYMLINK_NOFOLLOW | AT_EMPTY_PATH | AT_NO_AUTOMOUNT | AT_STATX_SYNC_AS_STAT
	        | AT_STATX_FORCE_SYNC | AT_STATX_DONT_SYNC)) {
		return EINVAL;
	}

	if (!(flags & AT_EMPTY_PATH) && (!pathname || !strlen(pathname))) {
		return ENOENT;
	}

	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;
	else {
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		memset(statxbuf, 0, sizeof(struct statx));

		switch (resp.file_type()) {
			case managarm::posix::FileType::FT_REGULAR:
				statxbuf->stx_mode = S_IFREG;
				break;
			case managarm::posix::FileType::FT_DIRECTORY:
				statxbuf->stx_mode = S_IFDIR;
				break;
			case managarm::posix::FileType::FT_SYMLINK:
				statxbuf->stx_mode = S_IFLNK;
				break;
			case managarm::posix::FileType::FT_CHAR_DEVICE:
				statxbuf->stx_mode = S_IFCHR;
				break;
			case managarm::posix::FileType::FT_BLOCK_DEVICE:
				statxbuf->stx_mode = S_IFBLK;
				break;
			case managarm::posix::FileType::FT_SOCKET:
				statxbuf->stx_mode = S_IFSOCK;
				break;
			case managarm::posix::FileType::FT_FIFO:
				statxbuf->stx_mode = S_IFIFO;
				break;
			default:
				__ensure(!resp.file_type());
		}

		statxbuf->stx_mask = mask; // TODO: Properly?
		// statxbuf->st_dev = 1;
		statxbuf->stx_ino = resp.fs_inode();
		statxbuf->stx_mode |= resp.mode();
		statxbuf->stx_nlink = resp.num_links();
		statxbuf->stx_uid = resp.uid();
		statxbuf->stx_gid = resp.gid();
		statxbuf->stx_rdev_major = major(resp.ref_devnum());
		statxbuf->stx_rdev_minor = minor(resp.ref_devnum());
		statxbuf->stx_size = resp.file_size();
		statxbuf->stx_atime.tv_sec = resp.atime_secs();
		statxbuf->stx_atime.tv_nsec = resp.atime_nanos();
		statxbuf->stx_mtime.tv_sec = resp.mtime_secs();
		statxbuf->stx_mtime.tv_nsec = resp.mtime_nanos();
		statxbuf->stx_ctime.tv_sec = resp.ctime_secs();
		statxbuf->stx_ctime.tv_nsec = resp.ctime_nanos();
		statxbuf->stx_blksize = 4096;
		statxbuf->stx_blocks = resp.file_size() / 512 + 1;
		return 0;
	}
}

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	return sys_readlinkat(AT_FDCWD, path, data, max_size, length);
}

int sys_readlinkat(int dirfd, const char *path, void *data, size_t max_size, ssize_t *length) {
	SignalGuard sguard;

	managarm::posix::ReadlinkAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

	auto [offer, send_head, send_tail, recv_resp, recv_data] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadTail(req, getSysdepsAllocator()),
	        helix_ng::recvInline(),
	        helix_ng::recvBuffer(data, max_size)
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_head.error());
	HEL_CHECK(send_tail.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*length = recv_data.actualLength();
	return 0;
}

int sys_rmdir(const char *path) {
	SignalGuard sguard;

	managarm::posix::RmdirRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_ftruncate(int fd, size_t size) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_TRUNCATE);
	req.set_size(size);

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
	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_fallocate(int fd, off_t offset, size_t size) {
	SignalGuard sguard;

	auto handle = getHandleForFd(fd);
	if (!handle)
		return EBADF;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::PT_FALLOCATE);
	req.set_rel_offset(offset);
	req.set_size(size);

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
	if (resp.error() != managarm::fs::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	SignalGuard sguard;

	managarm::posix::UnlinkAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_access(const char *path, int mode) { return sys_faccessat(AT_FDCWD, path, mode, 0); }

int sys_faccessat(int dirfd, const char *pathname, int, int flags) {
	SignalGuard sguard;

	managarm::posix::AccessAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	req.set_fd(dirfd);
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_flock(int fd, int opts) {
	SignalGuard sguard;

	managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_req_type(managarm::fs::CntReqType::FLOCK);
	req.set_fd(fd);
	req.set_flock_flags(opts);
	auto handle = getHandleForFd(fd);
	if (!handle) {
		return EBADF;
	}

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    handle,
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_isatty(int fd) {
	SignalGuard sguard;

	managarm::posix::IsTtyRequest<MemoryAllocator> req(getSysdepsAllocator());
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
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (resp.mode())
		return 0;
	return ENOTTY;
}

int sys_chmod(const char *pathname, mode_t mode) {
	return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}

int sys_fchmod(int fd, mode_t mode) { return sys_fchmodat(fd, "", mode, AT_EMPTY_PATH); }

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	SignalGuard sguard;

	managarm::posix::FchmodAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	req.set_mode(mode);
	req.set_flags(flags);

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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
	(void)dirfd;
	(void)pathname;
	(void)owner;
	(void)group;
	(void)flags;
	mlibc::infoLogger() << "mlibc: sys_fchownat is a stub!" << frg::endlog;
	return 0;
}

int sys_umask(mode_t mode, mode_t *old) {
	(void)mode;
	mlibc::infoLogger() << "mlibc: sys_umask is a stub, hardcoding 022!" << frg::endlog;
	*old = 022;
	return 0;
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	SignalGuard sguard;

	managarm::posix::UtimensAtRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(dirfd);
	if (pathname != nullptr)
		req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), pathname));
	if (times) {
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	return 0;
}

int sys_getentropy(void *buffer, size_t length) {
	SignalGuard sguard;
	auto p = reinterpret_cast<char *>(buffer);
	size_t n = 0;

	while (n < length) {
		size_t chunk;
		HEL_CHECK(helGetRandomBytes(p + n, length - n, &chunk));
		n += chunk;
	}

	return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	SignalGuard sguard;
	mlibc::infoLogger() << "mlibc: gethostname always returns managarm" << frg::endlog;
	char name[10] = "managarm\0";
	if (bufsize < 10)
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

	managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*fd = resp.fd();
	return 0;
}

int sys_uname(struct utsname *buf) {
	__ensure(buf);
	mlibc::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frg::endlog;
	strcpy(buf->sysname, "Managarm");
	strcpy(buf->nodename, "managarm");
	strcpy(buf->release, "0.0.1-rolling");
	strcpy(buf->version, "Managarm is not Managram");
#if defined(__x86_64__)
	strcpy(buf->machine, "x86_64");
#elif defined(__aarch64__)
	strcpy(buf->machine, "aarch64");
#elif defined(__riscv)
	strcpy(buf->machine, "riscv64");
#else
#error Unknown architecture
#endif

	return 0;
}

int sys_madvise(void *, size_t, int) {
	mlibc::infoLogger() << "mlibc: sys_madvise is a stub!" << frg::endlog;
	return 0;
}

int sys_ptsname(int fd, char *buffer, size_t length) {
	int index;
	if (int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if ((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

int sys_unlockpt(int fd) {
	int unlock = 0;

	if (int e = sys_ioctl(fd, TIOCSPTLCK, &unlock, NULL); e)
		return e;

	return 0;
}

int sys_getrlimit(int resource, struct rlimit *limit) {
	switch (resource) {
		case RLIMIT_NOFILE:
			/* TODO: change this once we support more than 512 */
			limit->rlim_cur = 512;
			limit->rlim_max = 512;
			return 0;
		default:
			return EINVAL;
	}
}

int sys_sysconf(int num, long *ret) {
	switch (num) {
		case _SC_OPEN_MAX: {
			struct rlimit ru;
			if (int e = sys_getrlimit(RLIMIT_NOFILE, &ru); e) {
				return e;
			}
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_PHYS_PAGES:
		case _SC_AVPHYS_PAGES: {
			// defer these to the generic implementation.
			return EINVAL;
		}
		default: {
			SignalGuard sguard;

			managarm::posix::SysconfRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_num(num);

			auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			    getPosixLane(),
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
			    )
			);

			HEL_CHECK(offer.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::posix::SysconfResponse<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() != managarm::posix::Errors::SUCCESS)
				return resp.error() | toErrno;

			*ret = resp.value();
			return 0;
		}
	}

	return 0;
}

int sys_sysinfo(struct sysinfo *info) {
	SignalGuard sguard;

	managarm::posix::GetMemoryInformationRequest<MemoryAllocator> req(getSysdepsAllocator());

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);

	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::GetMemoryInformationResponse resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	FILE *uptime = fopen("/proc/uptime", "r");
	__ensure(uptime);
	int uptime_sec;
	int uptime_msec;
	int idle_sec;
	int idle_msec;
	fscanf(uptime, "%d.%d %d.%d", &uptime_sec, &uptime_msec, &idle_sec, &idle_msec);
	fclose(uptime);

	// TODO: fill in missing fields.
	*info = {};
	info->uptime = uptime_sec;
	info->totalram = resp.total_usable_memory();
	info->freeram = resp.available_memory();
	info->mem_unit = resp.memory_unit();

	return 0;
}

int sys_fstatfs(int fd, struct statfs *buf) {
	SignalGuard sguard;

	managarm::posix::FstatfsRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(fd);

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

	managarm::posix::FstatfsResponse resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	memset(buf, NULL, sizeof(struct statfs));
	buf->f_type = resp.fstype();
	return 0;
}

int sys_prctl(int option, va_list va, int *out) {
	switch (option) {
		case PR_CAPBSET_READ:
			// TODO: Implement PR_CAPBSET read if we ever support capabilities
			*out = 1;
			return 0;
		case PR_SET_NAME: {
			const auto name = va_arg(va, char *);
			*out = 0;
			return pthread_setname_np(pthread_self(), name);
		}
		case PR_GET_NAME: {
			const auto name = va_arg(va, char *);
			*out = 0;
			return pthread_getname_np(pthread_self(), name, 16);
		}
		case PR_SET_PDEATHSIG: {
			managarm::posix::ParentDeathSignalRequest<MemoryAllocator> req{getSysdepsAllocator()};
			const auto value = va_arg(va, int);
			req.set_signal(value);

			auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			    getPosixLane(),
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
			    )
			);

			HEL_CHECK(offer.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::posix::ParentDeathSignalResponse resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() != managarm::posix::Errors::SUCCESS)
				return resp.error() | toErrno;
			*out = 0;
			return 0;
		}
		default:
			mlibc::infoLogger() << "mlibc: prctl: operation: " << option << " unimplemented!"
			                    << frg::endlog;
			return EINVAL;
	}
}

int sys_statfs(const char *path, struct statfs *buf) {
	SignalGuard sguard;

	managarm::posix::FstatfsRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_fd(-1);
	req.set_path(frg::string<MemoryAllocator>(getSysdepsAllocator(), path));

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

	managarm::posix::FstatfsResponse resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());

	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	memset(buf, NULL, sizeof(struct statfs));
	buf->f_type = resp.fstype();
	return 0;
}

// We don't support extended attributes yet
int sys_removexattr(const char *, const char *) { return ENOSYS; }

int sys_lgetxattr(const char *, const char *, void *, size_t, ssize_t *) { return ENOSYS; }

int sys_setxattr(const char *, const char *, const void *, size_t, int) { return ENOSYS; }

// We don't implement name_to_handle_at
int sys_name_to_handle_at(int, const char *, struct file_handle *, int *, int) { return ENOSYS; }

} // namespace mlibc
