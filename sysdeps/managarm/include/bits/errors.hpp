#pragma once

#include <errno.h>
#include <mlibc/debug.hpp>

#include "fs.frigg_bragi.hpp"
#include "posix.frigg_bragi.hpp"

struct ToErrno {
	template <typename E>
	auto operator()(E e) const {
		return e | *this;
	}
};
constexpr ToErrno toErrno;

inline int operator|(managarm::fs::Errors e, ToErrno) {
	switch (e) {
		case managarm::fs::Errors::SUCCESS:
			return 0;
		case managarm::fs::Errors::FILE_NOT_FOUND:
			return ENOENT;
		case managarm::fs::Errors::END_OF_FILE:
			return 0;
		case managarm::fs::Errors::ILLEGAL_ARGUMENT:
			return EINVAL;
		case managarm::fs::Errors::WOULD_BLOCK:
			return EAGAIN;
		case managarm::fs::Errors::SEEK_ON_PIPE:
			return ESPIPE;
		case managarm::fs::Errors::BROKEN_PIPE:
			return EPIPE;
		case managarm::fs::Errors::ACCESS_DENIED:
			return EPERM;
		case managarm::fs::Errors::NOT_DIRECTORY:
			return ENOTDIR;
		case managarm::fs::Errors::AF_NOT_SUPPORTED:
			return EAFNOSUPPORT;
		case managarm::fs::Errors::DESTINATION_ADDRESS_REQUIRED:
			return EDESTADDRREQ;
		case managarm::fs::Errors::NETWORK_UNREACHABLE:
			return ENETUNREACH;
		case managarm::fs::Errors::MESSAGE_TOO_LARGE:
			return EMSGSIZE;
		case managarm::fs::Errors::HOST_UNREACHABLE:
			return EHOSTUNREACH;
		case managarm::fs::Errors::INSUFFICIENT_PERMISSIONS:
			return EPERM;
		case managarm::fs::Errors::ADDRESS_IN_USE:
			return EADDRINUSE;
		case managarm::fs::Errors::ADDRESS_NOT_AVAILABLE:
			return EADDRNOTAVAIL;
		case managarm::fs::Errors::NOT_CONNECTED:
			return ENOTCONN;
		case managarm::fs::Errors::ALREADY_EXISTS:
			return EEXIST;
		case managarm::fs::Errors::ILLEGAL_OPERATION_TARGET:
			return EINVAL;
		case managarm::fs::Errors::NO_SPACE_LEFT:
			return ENOSPC;
		case managarm::fs::Errors::NOT_A_TERMINAL:
			return ENOTTY;
		case managarm::fs::Errors::NO_BACKING_DEVICE:
			return ENXIO;
		case managarm::fs::Errors::IS_DIRECTORY:
			return EISDIR;
		case managarm::fs::Errors::INVALID_PROTOCOL_OPTION:
			return ENOPROTOOPT;
		case managarm::fs::Errors::DIRECTORY_NOT_EMPTY:
			return ENOTEMPTY;
		case managarm::fs::Errors::CONNECTION_REFUSED:
			return ECONNREFUSED;
		case managarm::fs::Errors::ALREADY_CONNECTED:
			return EISCONN;
		case managarm::fs::Errors::INTERNAL_ERROR:
			return EIO;
	}

	mlibc::panicLogger() << "unhandled managarm::fs::Errors " << static_cast<int32_t>(e)
	                     << frg::endlog;
	__builtin_unreachable();
}

inline int operator|(managarm::posix::Errors e, ToErrno) {
	switch (e) {
		case managarm::posix::Errors::SUCCESS:
			return 0;
		case managarm::posix::Errors::FILE_NOT_FOUND:
			return ENOENT;
		case managarm::posix::Errors::END_OF_FILE:
			return 0;
		case managarm::posix::Errors::WOULD_BLOCK:
			return EAGAIN;
		case managarm::posix::Errors::BROKEN_PIPE:
			return EPIPE;
		case managarm::posix::Errors::ACCESS_DENIED:
			return EPERM;
		case managarm::posix::Errors::NOT_A_DIRECTORY:
			return ENOTDIR;
		case managarm::posix::Errors::INSUFFICIENT_PERMISSION:
			return EPERM;
		case managarm::posix::Errors::ALREADY_EXISTS:
			return EEXIST;
		case managarm::posix::Errors::ILLEGAL_OPERATION_TARGET:
			return EINVAL;
		case managarm::posix::Errors::NO_BACKING_DEVICE:
			return ENXIO;
		case managarm::posix::Errors::IS_DIRECTORY:
			return EISDIR;
		case managarm::posix::Errors::DIRECTORY_NOT_EMPTY:
			return ENOTEMPTY;
		case managarm::posix::Errors::INTERNAL_ERROR:
			return EIO;
		case managarm::posix::Errors::DEAD_FORK:
			return EAGAIN;
		case managarm::posix::Errors::ILLEGAL_REQUEST:
			return ENOSYS;
		case managarm::posix::Errors::ILLEGAL_ARGUMENTS:
			return EINVAL;
		case managarm::posix::Errors::NO_SUCH_FD:
			return EBADF;
		case managarm::posix::Errors::BAD_FD:
			return EBADFD;
		case managarm::posix::Errors::NOT_SUPPORTED:
			return ENOTSUP;
		case managarm::posix::Errors::RESOURCE_IN_USE:
			return EBUSY;
		case managarm::posix::Errors::NO_SUCH_RESOURCE:
			return ESRCH;
		case managarm::posix::Errors::NOT_A_TTY:
			return ENOTTY;
		case managarm::posix::Errors::PROTOCOL_NOT_SUPPORTED:
			return EPROTONOSUPPORT;
		case managarm::posix::Errors::ADDRESS_FAMILY_NOT_SUPPORTED:
			return EAFNOSUPPORT;
		case managarm::posix::Errors::NO_MEMORY:
			return ENOMEM;
		case managarm::posix::Errors::NO_CHILD_PROCESSES:
			return ECHILD;
		case managarm::posix::Errors::SYMBOLIC_LINK_LOOP:
			return ELOOP;
		case managarm::posix::Errors::ALREADY_CONNECTED:
			return EISCONN;
		case managarm::posix::Errors::UNSUPPORTED_SOCKET_TYPE:
			return ESOCKTNOSUPPORT;
	}

	mlibc::panicLogger() << "unhandled managarm::posix::Errors " << static_cast<int32_t>(e)
	                     << frg::endlog;
	__builtin_unreachable();
}
