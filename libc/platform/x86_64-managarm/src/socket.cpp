
#include <errno.h>
#include "sys/socket.h"

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>

int connect(int fd, const struct sockaddr *address, socklen_t addr_length) {
	managarm::posix::ClientRequest<MemoryAllocator> request(getAllocator());
	request.set_request_type(managarm::posix::ClientRequestType::CONNECT);
	request.set_fd(fd);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(getAllocator());
	request.SerializeToString(&serialized);
	HelError error;
	posixPipe.sendStringReqSync(serialized.data(), serialized.size(),
			eventHub, request_num, 0, error);
	HEL_CHECK(error);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe.recvStringRespSync(buffer, 128, eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(getAllocator());
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::SUCCESS) {
		return 0;
	}else if(response.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;		
		return -1;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}
