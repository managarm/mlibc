
#include <string.h>
#include <errno.h>
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
	}
}

