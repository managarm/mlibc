
#include <string.h>
#include <errno.h>

// for dup2()
#include <unistd.h>
// for open()
#include <fcntl.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#pragma GCC visibility push(hidden)

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>

#pragma GCC visibility pop

int open(const char *path, int flags, ...) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::OPEN);
	request.set_path(frigg::String<MemoryAllocator>(*memoryAllocator, path));

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::FILE_NOT_FOUND) {
		errno = ENOENT;
		return -1;
	}else if(response.error() == managarm::posix::Errors::SUCCESS) {
		return response.fd();
	}else{
		__ensure(!"Unexpected error in open()!");
		__builtin_unreachable();
	}
}

ssize_t write(int fd, const void *buffer, size_t size) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::WRITE);
	request.set_fd(fd);
	request.set_buffer(frigg::String<MemoryAllocator>(*memoryAllocator,
			(const char *)buffer, size));

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	uint8_t msg_buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(msg_buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(msg_buffer, length);
	if(response.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else if(response.error() == managarm::posix::Errors::SUCCESS) {
		return size;
	}else{
		__ensure(!"Unexpected error in write()!");
		__builtin_unreachable();
	}
}

int close(int fd) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::CLOSE);
	request.set_fd(fd);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	uint8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;		
		return -1;
	}else if(response.error() == managarm::posix::Errors::SUCCESS) {
		return 0;
	}else{
		__ensure(!"Unexpected error in close()!");
		__builtin_unreachable();
	}
}

int dup2(int src_fd, int dest_fd) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::DUP2);
	request.set_fd(src_fd);
	request.set_newfd(dest_fd);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::SUCCESS) {
		return dest_fd;
	}else if(response.error() ==  managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else {
		__ensure(!"Unexpected error in dup2()!");
		__builtin_unreachable();
	}
}

int isatty(int fd) {
	frigg::infoLogger.log() << "mlibc: Broken isatty() called!" << frigg::EndLog();
	return 1;
}

char *ttyname(int) {
	frigg::infoLogger.log() << "mlibc: Broken ttyname() called!" << frigg::EndLog();
	return "/dev/pts/1";
}

