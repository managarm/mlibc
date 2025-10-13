#ifndef MLIBC_FILE_WINDOW
#define MLIBC_FILE_WINDOW

#include <abi-bits/fcntl.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/internal-sysdeps.hpp>
#include <internal-config.h>

struct file_window {
	file_window(const char *path) {
		int fd;
		if(mlibc::sys_open(path, O_RDONLY, 0, &fd))
			mlibc::panicLogger() << "mlibc: Error opening file_window to "
					<< path << frg::endlog;

		if(!mlibc::sys_stat) {
			MLIBC_MISSING_SYSDEP();
			__ensure(!"cannot proceed without sys_stat");
		}
		struct stat info;
		if(mlibc::sys_stat(mlibc::fsfd_target::fd, fd, "", 0, &info))
			mlibc::panicLogger() << "mlibc: Error getting stats for " << path << frg::endlog;

		size_ = info.st_size;

#if MLIBC_MAP_FILE_WINDOWS
		if(mlibc::sys_vm_map(nullptr, size_, PROT_READ, MAP_PRIVATE,
				fd, 0, &_ptr))
			mlibc::panicLogger() << "mlibc: Error mapping file_window to " << path << frg::endlog;
#else
		_ptr = getAllocator().allocate(size_);
		__ensure(_ptr);

		size_t progress = 0;
		while(progress < size_) {
			ssize_t chunk;
			if(int e = mlibc::sys_read(fd, reinterpret_cast<char *>(_ptr) + progress,
					size_ - progress, &chunk); e)
				mlibc::panicLogger() << "mlibc: Read from file_window failed" << frg::endlog;
			if(!chunk)
				break;
			progress += chunk;
		}
		if(progress != size_)
			mlibc::panicLogger() << "stat reports " << size_ << " but we only read "
					<< progress << " bytes" << frg::endlog;
#endif

		if(mlibc::sys_close(fd))
			mlibc::panicLogger() << "mlibc: Error closing file_window to " << path << frg::endlog;
	}

	~file_window() {
#if MLIBC_MAP_FILE_WINDOWS
		if (mlibc::sys_vm_unmap(_ptr, size_))
			mlibc::panicLogger() << "mlibc: Error unmapping file_window" << frg::endlog;
#else
		getAllocator().deallocate(_ptr, size_);
#endif
	}

	void *get() const {
		return _ptr;
	}

	size_t size() const {
		return size_;
	}

private:
	void *_ptr;
	size_t size_;
};

#endif // MLIBC_FILE_WINDOW

