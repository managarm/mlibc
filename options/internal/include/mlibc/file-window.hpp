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
			mlibc::panicLogger() << "mlibc: Error getting TZinfo stats" << frg::endlog;

#if MLIBC_MAP_FILE_WINDOWS
		if(mlibc::sys_vm_map(nullptr, (size_t)info.st_size, PROT_READ, MAP_PRIVATE,
				fd, 0, &_ptr))
			mlibc::panicLogger() << "mlibc: Error mapping TZinfo" << frg::endlog;
#else
		_ptr = getAllocator().allocate(info.st_size);
		__ensure(_ptr);

		size_t progress = 0;
		size_t st_size = static_cast<size_t>(info.st_size);
		while(progress < st_size) {
			ssize_t chunk;
			if(int e = mlibc::sys_read(fd, reinterpret_cast<char *>(_ptr) + progress,
					st_size - progress, &chunk); e)
				mlibc::panicLogger() << "mlibc: Read from file_window failed" << frg::endlog;
			if(!chunk)
				break;
			progress += chunk;
		}
		if(progress != st_size)
			mlibc::panicLogger() << "stat reports " << info.st_size << " but we only read "
					<< progress << " bytes" << frg::endlog;
#endif

		if(mlibc::sys_close(fd))
			mlibc::panicLogger() << "mlibc: Error closing TZinfo" << frg::endlog;
	}

	// TODO: Write destructor to deallocate/unmap memory.

	void *get() {
		return _ptr;
	}

private:
	void *_ptr;
};

#endif // MLIBC_FILE_WINDOW

