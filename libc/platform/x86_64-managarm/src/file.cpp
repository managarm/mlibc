
#include <string.h>
#include <errno.h>

// for dup2()
#include <unistd.h>
// for open()
#include <fcntl.h>
// for tcgetattr()
#include <termios.h>
// for stat()
#include <sys/stat.h>

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

int stat(const char *__restrict path, struct stat *__restrict result) {
	int fd = open(path, O_RDONLY);
	if(fd == -1) {
		__ensure(errno == ENOENT);
		return -1;
	}

	if(fstat(fd, result))
		__ensure("Could not fstat() internal file");

	if(close(fd))
		__ensure("Could not close() internal file");
	return 0;
}

int fstat(int fd, struct stat *result) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::FSTAT);
	request.set_fd(fd);

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
		memset(result, 0, sizeof(struct stat));
		result->st_dev = 1;
		result->st_ino = response.inode_num();
		result->st_mode = response.mode() | S_IFREG;
		result->st_nlink = response.num_links();
		result->st_uid = response.uid();
		result->st_gid = response.gid();
		result->st_rdev = 0;
		result->st_size = response.file_size();
		result->st_atim.tv_sec = response.atime_secs();
		result->st_atim.tv_nsec = response.atime_nanos();
		result->st_mtim.tv_sec = response.mtime_secs();
		result->st_mtim.tv_nsec = response.mtime_nanos();
		result->st_ctim.tv_sec = response.ctime_secs();
		result->st_ctim.tv_nsec = response.ctime_nanos();
		result->st_blksize = 4096;
		result->st_blocks = response.file_size() / 512 + 1;
		return 0;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

int open(const char *path, int flags, ...) {
	frigg::infoLogger.log() << "mlibc: open(\""
			<< path << "\") called!" << frigg::EndLog();
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
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

ssize_t read(int fd, void *buffer, size_t size){
//	frigg::infoLogger.log() << "read()" << frigg::EndLog();
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::READ);
	request.set_fd(fd);
	request.set_size(size);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	posixPipe->sendStringReq(serialized.data(), serialized.size(),
			request_num, 0);

	uint8_t msg_buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(msg_buffer, 128, *eventHub,
			request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(msg_buffer, length);
	if(response.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else if(response.error() == managarm::posix::Errors::END_OF_FILE) {
		return 0;
	}
	assert(response.error() == managarm::posix::Errors::SUCCESS);
	
	size_t data_length;
	HelError data_error;
	posixPipe->recvStringRespSync(buffer, size, *eventHub,
			request_num, 1, data_error, data_length);
	HEL_CHECK(data_error);
	return data_length;
};

ssize_t write(int fd, const void *buffer, size_t size) {
//	frigg::infoLogger.log() << "write()" << frigg::EndLog();
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
	posixPipe->recvStringRespSync(msg_buffer, 128, *eventHub,
			request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(msg_buffer, length);
	if(response.error() == managarm::posix::Errors::NO_SUCH_FD) {
		errno = EBADF;
		return -1;
	}else if(response.error() == managarm::posix::Errors::SUCCESS) {
		return size;
	}else{
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

off_t lseek(int fd, off_t offset, int whence) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_fd(fd);
	request.set_rel_offset(offset);

	if(whence == SEEK_SET) {
		request.set_request_type(managarm::posix::ClientRequestType::SEEK_ABS);
	}else if(whence == SEEK_CUR) {
		request.set_request_type(managarm::posix::ClientRequestType::SEEK_REL);
	}else if(whence == SEEK_END) {
		request.set_request_type(managarm::posix::ClientRequestType::SEEK_EOF);
	}else{
		frigg::panicLogger.log() << "Illegal whence argument" << frigg::EndLog();
	}

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
		return response.offset();
	}else{
		__ensure(!"Unexpected error");
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
		__ensure(!"Unexpected error");
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
		__ensure(!"Unexpected error");
		__builtin_unreachable();
	}
}

int fcntl(int, int, ...) {
	frigg::infoLogger.log() << "mlibc: Broken fcntl() called!" << frigg::EndLog();
	return 0;
}

int isatty(int fd) {
	frigg::infoLogger.log() << "mlibc: Broken isatty() called!" << frigg::EndLog();
	return 1;
}

char *ttyname(int) {
	frigg::infoLogger.log() << "mlibc: Broken ttyname() called!" << frigg::EndLog();
	return "/dev/pts/1";
}

int tcgetattr(int fd, struct termios *attr) {
	frigg::infoLogger.log() << "mlibc: Broken tcgetattr() called!" << frigg::EndLog();
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

int tcsetattr(int, int, const struct termios *attr) {
	frigg::infoLogger.log() << "mlibc: Broken tcsetattr("
			<< (void *)attr->c_iflag << ", " << (void *)attr->c_oflag
			<< ", " << (void *)attr->c_cflag << ", " << (void *)attr->c_lflag
			<< ") called!" << frigg::EndLog();
	return 0;
}

