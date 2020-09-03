#include <lemon/syscall.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

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
		long ret = syscall(SYS_WRITE, fd, (uintptr_t)buffer, count, 0, 0);

		if(ret < 0)
			return -ret;

		*written = ret;
		return 0;
	}

	int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    	long ret = syscall(SYS_READ, fd, (uintptr_t)buf, count, 0, 0);

		if(ret < 0){
			*bytes_read = 0;
			return -ret;
		}

		*bytes_read = ret;
		return 0;
	}
	
	int sys_pwrite(int fd, const void* buffer, size_t count, off_t off, ssize_t* written){
		int ret = syscall(SYS_PWRITE, fd, (uintptr_t)buffer, count, 0, off);


		if(ret < 0){
			return -ret;
		}

		*written = ret;
		return 0;
	}
	
	int sys_pread(int fd, void *buf, size_t count, off_t off, ssize_t *bytes_read) {
		int ret = syscall(SYS_PREAD, fd, (uintptr_t)buf, count, 0, off);

		if(ret < 0){
			return -ret;
		}

		*bytes_read = ret;
		return 0;
	}

	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		long ret = syscall(SYS_LSEEK, fd, offset, whence, 0, 0);

		if(ret < 0){
			return -ret;
		}

		*new_offset = ret;
		return 0;
	}


	int sys_open(const char* filename, int flags, int* fd){
		long ret = syscall(SYS_OPEN, (uintptr_t)filename, flags, 0, 0, 0);

		if(ret < 0)
			return -ret;

		*fd = ret;

		return 0; 
	}

	int sys_close(int fd){
		syscall(SYS_CLOSE, fd, 0, 0, 0, 0);
		return 0;
	}

	int sys_access(const char* filename, int mode){
		int fd;
		if(int e = sys_open(filename, O_RDONLY, &fd)){
			return e;
		}

		sys_close(fd);
		return 0;
	}
	
	int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
		int _fd = 0;
		long ret = 0;

		lemon_stat_t lemonStat;
		switch(fsfdt){
			case fsfd_target::fd:
				ret = syscall(SYS_FSTAT, &lemonStat, fd, 0, 0, 0);
				break;
			case fsfd_target::path:
				ret = syscall(SYS_STAT, &lemonStat, path, 0, 0, 0);
				break;
			default:
				mlibc::infoLogger() << "mlibc warning: sys_stat: unsupported fsfd target" << frg::endlog;
				return EINVAL;
		}

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

		return -ret;
	}

	int sys_ioctl(int fd, unsigned long request, void *arg, int *result){
		return syscall(SYS_IOCTL, fd, request, arg, result, 0);
	}

	#ifndef MLIBC_BUILDING_RTDL

	#define LEMON_TIOCGATTR 0xB301
	#define LEMON_TIOCSATTR 0xB302

	int sys_isatty(int fd) {
		struct winsize ws;
		long ret = syscall(SYS_IOCTL, fd, TIOCGWINSZ, &ws, 0 ,0);

		if(!ret) return 0;
		
		return ENOTTY;
	}

	int sys_tcgetattr(int fd, struct termios *attr) {
		if(int e = sys_isatty(fd))
			return e;

		int ret;
		sys_ioctl(fd, LEMON_TIOCGATTR, attr, &ret);

		if(ret)
			return -ret;

		return 0;
	}

	int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
		if(int e = sys_isatty(fd))
			return e;

		if(optional_action){
			mlibc::infoLogger() << "mlibc warning: sys_tcsetattr ignores optional_action" << frg::endlog;
		}

		int ret;
		sys_ioctl(fd, LEMON_TIOCSATTR, const_cast<struct termios*>(attr), &ret);

		if(ret)
			return -ret;

		return 0;
	}

	int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events){
		long ret = syscall(SYS_POLL, fds, count, timeout, 0, 0);

		if(ret < 0){
			return -ret;
		}

		*num_events = ret;

		return 0;
	}

	int sys_mkdir(const char* path){
		long ret = syscall(SYS_MKDIR, path, 0, 0, 0, 0);

		if(ret < 0){
			return -ret;
		}

		return 0;
	}

	int sys_rmdir(const char* path){
		long ret = syscall(SYS_RMDIR, path, 0, 0, 0, 0);

		if(ret < 0){
			return -ret;
		}

		return 0;
	}

	int sys_link(const char* srcpath, const char* destpath){
		long ret = syscall(SYS_LINK, srcpath, destpath, 0, 0, 0);

		if(ret < 0){
			return -ret;
		}

		return 0;
	}

	int sys_unlink(const char* path){
		long ret = syscall(SYS_UNLINK, path, 0, 0, 0, 0);

		if(ret < 0){
			return -ret;
		}

		return 0;
	}

	typedef struct lemon_dirent {
		uint32_t inode; // Inode number
		uint32_t type;
		char name[NAME_MAX]; // Filename
	} lemon_dirent_t;

	int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
		lemon_dirent_t lemonDirent;
		long ret = syscall(SYS_READDIR_NEXT, handle, &lemonDirent, 0, 0, 0);

		if(!ret){
			*bytes_read = 0;
			return 0;
		} else if(ret > 0){
			dirent* dir = (dirent*)buffer;
			strcpy(dir->d_name, lemonDirent.name);
			dir->d_ino = lemonDirent.inode;
			dir->d_off = 0;
			dir->d_reclen = sizeof(dirent);
			dir->d_type = lemonDirent.type;

			*bytes_read = sizeof(dirent);
			return 0;
		} else {
			return -ret;
		}
	}

	int sys_open_dir(const char* path, int* handle){
		return sys_open(path, O_DIRECTORY, handle);
	}

	int sys_rename(const char* path, const char* new_path){
		return -syscall(SYS_RENAME, path, new_path, 0, 0, 0);
	}
	
	int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length){
		long ret = syscall(SYS_READLINK, path, buffer, max_size, 0, 0);
		if(ret < 0){
			return -ret;
		}

		*length = ret;
		return 0;
	}
	#endif
} 
