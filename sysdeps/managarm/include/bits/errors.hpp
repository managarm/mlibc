#pragma once

#include <errno.h>

#include "fs.frigg_bragi.hpp"

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
		case managarm::fs::Errors::INTERNAL_ERROR:
			return EIO;
	}
}
