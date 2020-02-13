#include <lemon/filesystem.h>

namespace mlibc{

	int sys_write(int fd, const void* buffer, size_t count, ssize_t* written){
		ssize_t _written = lemon_write(fd, buffer, count);

		if(_written == -1)
			return -1;

		*written = _written;
		return 0;
	}

	int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
		ssize_t ret;
		int sys_errno;

		ret = lemon_read(fd, buf, count);

		if (ret == -1)
		    return -1;

		*bytes_read = ret;
		return 0;
	}

	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		off_t off;
		int sys_errno;

		off = lemon_seek(fd, offset, whence);

		if (off == -1)
		    return -1;

		*new_offset = off;
		return 0;
	}


	int sys_open(const char* filename, int flags, int* fd){
		int _fd = lemon_open(filename, flags);

		if(!fd)
			return -1;

		*fd = _fd;
		return 0; 
	}

	#warning finish filesystem sysdeps
	int sys_close(int fd){
		lemon_close(fd);
		return 0;
	}

	#warning Properly Implement access()
	int sys_access(const char* filename, int mode){
		int ret = lemon_open(filename, 0);

		if(ret) {
		    lemon_close(ret);
		    return 0;
		} else return 1;
	}

	void sys_libc_log(const char*){

	}
} 
