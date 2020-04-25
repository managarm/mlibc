#include <lemon/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mlibc/sysdeps.hpp>

namespace mlibc{
	typedef struct {
		dev_t st_dev;
		ino_t st_ino;
		mode_t st_mode;
		nlink_t st_nlink;
		uid_t st_uid;
		uid_t st_gid;
		dev_t st_rdev;
		off_t st_size;
		int64_t st_blksize;
		int64_t st_blocks;
	} lemon_stat_t;

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
	
	int sys_pwrite(int fd, const void* buffer, size_t count, off_t off, ssize_t* written){
		int ret;
		syscall(SYS_PWRITE, fd, (uintptr_t)buffer, count, (uintptr_t)&ret, off);

		*written = ret;

		if(*written == -1)
			return -1;

		return 0;
	}
	
	int sys_pread(int fd, void *buf, size_t count, off_t off, ssize_t *bytes_read) {
		int ret;
		int sys_errno;

    	syscall(SYS_PREAD, fd, (uintptr_t)buf, count, (uintptr_t)&ret, off);

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
	
	int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
		int _fd = 0;

		switch(fsfdt){
			case fsfd_target::fd:
				_fd = fd;
				break;
			case fsfd_target::path:
				sys_open(path, 0, &_fd);
				break;
			default:
				return 1;
		}

		int ret;
		lemon_stat_t lemonStat;
		syscall(SYS_STAT, &lemonStat, _fd, &ret, 0, 0);

		statbuf->st_dev = lemonStat.st_dev;
		statbuf->st_ino = lemonStat.st_ino;
		statbuf->st_mode = lemonStat.st_mode;
		statbuf->st_nlink = lemonStat.st_nlink;
		statbuf->st_uid = lemonStat.st_uid;
		statbuf->st_gid = lemonStat.st_gid;
		statbuf->st_rdev = lemonStat.st_rdev;
		statbuf->st_size = lemonStat.st_size;
		statbuf->st_blksize = lemonStat.st_blksize;
		statbuf->st_blocks = lemonStat.st_blocks;

		if(fsfdt == fsfd_target::path) sys_close(_fd);

		return ret;
	}
} 
