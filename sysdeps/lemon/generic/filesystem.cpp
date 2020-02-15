#include <lemon/syscall.h>
#include <sys/types.h>

namespace mlibc{

	int sys_write(int fd, const void* buffer, size_t count, ssize_t* written){


		int ret;
		syscall(SYS_WRITE, fd, (uintptr_t)buffer, count, (uintptr_t)&ret, 0);

		*written = ret;

		if(*written == -1)
			return -1;

		return 0;
	}

	int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
		int ret;
		int sys_errno;

    	syscall(SYS_READ, fd, (uintptr_t)buf, count, (uintptr_t)&ret, 0);

		if (ret == -1)
		    return -1;

		*bytes_read = ret;
		return 0;
	}

	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		off_t off;
		int sys_errno;

		uint64_t ret;
    	syscall(SYS_LSEEK, fd, offset, whence, (uintptr_t)&ret, 0);

		off = ret;

		if (off == -1)
		    return -1;

		*new_offset = off;
		return 0;
	}


	int sys_open(const char* filename, int flags, int* fd){
		int _fd;
		syscall(SYS_OPEN, (uintptr_t)filename, (uintptr_t)&_fd, 0, 0, 0);

		if(!fd)
			return -1;

		*fd = _fd;
		return 0; 
	}

	int sys_close(int fd){
		syscall(SYS_CLOSE, fd, 0, 0, 0, 0);
		return 0;
	}

	int sys_access(const char* filename, int mode){
		int ret;
		sys_open(filename, mode, &ret);

		if(ret) {
		    sys_close(ret);
		    return 0;
		} else return 1;
	}

	void sys_libc_log(const char*){

	}
} 
