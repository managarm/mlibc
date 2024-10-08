#include <asm-generic/errno.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/errors.hpp>

#include <hel.h>
#include <hel-syscalls.h>

#include <posix.frigg_bragi.hpp>
#include <fs.frigg_bragi.hpp>

int posixErrorToError(managarm::posix::Errors posixError) {
	using enum managarm::posix::Errors;

	switch(posixError) {
	case SUCCESS: return 0;
	case DEAD_FORK: mlibc::panicLogger() << "mlibc: posix error DEAD_FORK must be handled manually" << frg::endlog; __builtin_unreachable();
	case ILLEGAL_REQUEST: mlibc::panicLogger() << "mlibc: posix error ILLEGAL_REQUEST must be handled manually" << frg::endlog; __builtin_unreachable();
	case ILLEGAL_ARGUMENTS: return EINVAL;
	case FILE_NOT_FOUND: return ENOENT;
	case ACCESS_DENIED: return EPERM;
	case ALREADY_EXISTS: return EEXIST;
	case NO_SUCH_FD: return EBADFD;
	case END_OF_FILE: mlibc::panicLogger() << "mlibc: posix error END_OF_FILE must be handled manually" << frg::endlog; __builtin_unreachable();
	case BAD_FD: return EBADFD;
	case WOULD_BLOCK: return EWOULDBLOCK;
	case BROKEN_PIPE: return EPIPE;
	case NOT_SUPPORTED: return ENOTSUP;
	case RESOURCE_IN_USE: return EBUSY;
	case ILLEGAL_OPERATION_TARGET: return EINVAL;
	case NOT_A_DIRECTORY: return ENOTDIR;
	case NO_BACKING_DEVICE: return ENXIO;

	// TODO: the name of this posix error implies that it might return other errors, since "ESRCH" is "No such process"
	// Maybe we should rename this?
	case NO_SUCH_RESOURCE: return ESRCH;
	case INSUFFICIENT_PERMISSION: return EPERM;
	case IS_DIRECTORY: return EISDIR;
	case NOT_A_TTY: return ENOTTY;
	case PROTOCOL_NOT_SUPPORTED: return EPROTONOSUPPORT;
	case ADDRESS_FAMILY_NOT_SUPPORTED: return EAFNOSUPPORT;
	case NO_MEMORY: return ENOMEM;
	case INTERNAL_ERROR: return EIEIO;
	default: mlibc::panicLogger() << "mlibc: invalid posix error: " << (int)posixError << frg::endlog; __builtin_unreachable();
	}
}

int fsErrorToError(managarm::fs::Errors fsError) {
	using enum managarm::fs::Errors;

	switch(fsError) {
	case SUCCESS: return 0;
	case FILE_NOT_FOUND: return ENOENT;
	case END_OF_FILE: mlibc::panicLogger() << "mlibc: posix error END_OF_FILE must be handled manually" << frg::endlog; __builtin_unreachable();
	case ILLEGAL_ARGUMENT: return EINVAL;
	case WOULD_BLOCK: return EWOULDBLOCK;
	case SEEK_ON_PIPE: return ESPIPE;
	case BROKEN_PIPE: return EPIPE;
	case ACCESS_DENIED: return EPERM;
	case AF_NOT_SUPPORTED: return EAFNOSUPPORT;
	case DESTINATION_ADDRESS_REQUIRED: return EDESTADDRREQ;
	case NETWORK_UNREACHABLE: return ENETUNREACH;
	case MESSAGE_TOO_LARGE: return EMSGSIZE;
	case HOST_UNREACHABLE: return EHOSTUNREACH;
	case INSUFFICIENT_PERMISSIONS: return EPERM;
	case ADDRESS_IN_USE: return EADDRINUSE;
	case ADDRESS_NOT_AVAILABLE: return EADDRNOTAVAIL;
	case NOT_CONNECTED: return ENOTCONN;
	case ALREADY_EXISTS: return EEXIST;
	case ILLEGAL_OPERATION_TARGET: return EINVAL;
	case NOT_DIRECTORY: return ENOTDIR;
	case NO_SPACE_LEFT: return ENOSPC;
	case NOT_A_TERMINAL: return ENOTTY;
	case NO_BACKING_DEVICE: return ENXIO;
	case IS_DIRECTORY: return EISDIR;
	case NOT_A_SOCKET: return ENOTSOCK;
	default: mlibc::panicLogger() << "mlibc: invalid fs error: " << (int)posixError << frg::endlog; __builtin_unreachable();
	}
}
